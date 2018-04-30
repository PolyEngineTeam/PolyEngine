// include the basic windows header file
#include <SDL.h>

#include <Engine.hpp>
#include <LibraryLoader.hpp>
#include <AssetsPathConfig.hpp>

extern "C"
{
	using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (SDL_Window*, const Poly::ScreenSize&);
	using CreateGameFunc = Poly::IGame* (void);
}

void HandleWindowEvent(const SDL_WindowEvent& windowEvent);

enum eMouseStateChange {
	WINDOW_ENTER,
	WINDOW_LEAVE,
	BUTTON_CLICK,
	_COUNT
};

void UpdateMouseState(eMouseStateChange change);

class FileAndCoutStream : public Poly::FileOutputStream
{
public:
	FileAndCoutStream(const char* name) : Poly::FileOutputStream(name) {}
	void Append(const char* data) override {
		std::cout << data;
		std::cout.flush();
		Poly::FileOutputStream::Append(data);
	}
};

int main(int argc, char* args[])
{
	Poly::gConsole.RegisterStream<FileAndCoutStream>("console.log");
	UNUSED(argc);
	UNUSED(args);
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		ASSERTE(false, "SDL initialization failed!");
		return 1;
	}
	Poly::gConsole.LogDebug("SDL initialized.");

	// Initial screen size
	Poly::ScreenSize screenSize;
	screenSize.Width = 1920; // 800; // 256; // 1280;
	screenSize.Height = 1080; // 600; // 192; // 720;

	// Main standalone window
	SDL_Window* window = SDL_CreateWindow(
		"PolyEngine standalone",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		screenSize.Width,
		screenSize.Height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	ASSERTE(window, "Failed to create standalone window!");
	Poly::gConsole.LogDebug("Window created.");
	
	std::unique_ptr<Poly::Engine> Engine = std::make_unique<Poly::Engine>();
	Poly::gConsole.LogDebug("Engine created.");

	// Load rendering device library
	auto loadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>(Poly::gAssetsPathConfig.GetRenderingDeviceLibPath().GetCStr(), "PolyCreateRenderingDevice");
	if (!loadRenderingDevice.FunctionValid())
		return 1;
	Poly::gConsole.LogDebug("Library libRenderingDevice loaded.");

	// Load game library
	auto loadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>(Poly::gAssetsPathConfig.GetGameLibPath().GetCStr(), "CreateGame");
	if (!loadGame.FunctionValid())
		return 1;
	Poly::gConsole.LogDebug("Library libGame loaded.");
		
	std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(loadGame());
	Poly::gConsole.LogDebug("Game created");

	std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(loadRenderingDevice(window, screenSize));
	Poly::gConsole.LogDebug("Device created.");

	Engine->Init(std::move(game), std::move(device));
	Poly::gConsole.LogDebug("Engine initialization and handshake successfull. Starting main loop...");

	bool quitRequested = false;
	while (!quitRequested)
	{
		// Handle events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quitRequested = true;
				break;
			case SDL_KEYDOWN:
				Engine->KeyDown(static_cast<Poly::eKey>(event.key.keysym.scancode));
				break;
			case SDL_KEYUP:
				Engine->KeyUp(static_cast<Poly::eKey>(event.key.keysym.scancode));
				break;
			case SDL_MOUSEMOTION:
			{
				Engine->UpdateMouseMove(Poly::Vector2i(event.motion.xrel, event.motion.yrel));
				Engine->UpdateMousePos(Poly::Vector2i(event.motion.x, event.motion.y));
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
				Engine->MouseButtonDown(static_cast<Poly::eMouseButton>(event.button.button));
				UpdateMouseState(eMouseStateChange::BUTTON_CLICK);
				break;
			case SDL_MOUSEBUTTONUP:
				Engine->MouseButtonUp(static_cast<Poly::eMouseButton>(event.button.button));
				break;
			case SDL_MOUSEWHEEL:
				// Not sure if this is correct.
				Engine->UpdateWheelPos(Poly::Vector2i(event.wheel.x, event.wheel.y));
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				Engine->ControllerButtonDown(event.cbutton.which, static_cast<Poly::eControllerButton>(event.cbutton.button));
				break;
			case SDL_CONTROLLERBUTTONUP:
				Engine->ControllerButtonUp(event.cbutton.which, static_cast<Poly::eControllerButton>(event.cbutton.button));
				break;
			case SDL_CONTROLLERAXISMOTION:
				Engine->ControllerAxisMotion(event.caxis.which, static_cast<Poly::eControllerAxis>(event.caxis.axis), event.caxis.value);
				break;
			case SDL_CONTROLLERDEVICEADDED:
			{
				SDL_GameController* controller = SDL_GameControllerOpen(event.cdevice.which);
				SDL_Joystick* joystickPtr = SDL_GameControllerGetJoystick(controller);
				i32 joystickID = SDL_JoystickInstanceID(joystickPtr);
				Engine->AddController(joystickID);
				break;
			}
			case SDL_CONTROLLERDEVICEREMOVED:
			{
				SDL_GameController* controller = SDL_GameControllerFromInstanceID(event.cdevice.which);
				SDL_GameControllerClose(controller);
				Engine->RemoveController(event.cdevice.which);
				break;
			}
			case SDL_WINDOWEVENT:
				HandleWindowEvent(event.window);
				break;
			default:
				break;
			}
		}

		// Engine loop
		Engine->Update();
		quitRequested = quitRequested || Engine->IsQuitRequested();
	}
	Poly::gConsole.LogDebug("Closing main loop...");
	
	Engine.reset();

	// Destroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();
	Poly::gConsole.LogDebug("Exiting...");
	return 0;
}

const char* GetWindowEventName(const SDL_WindowEvent& windowEvent)
{
	switch(windowEvent.event)
	{
		case SDL_WINDOWEVENT_SHOWN: return "SDL_WINDOWEVENT_SHOWN";
		case SDL_WINDOWEVENT_HIDDEN: return "SDL_WINDOWEVENT_HIDDEN";
		case SDL_WINDOWEVENT_EXPOSED: return "SDL_WINDOWEVENT_EXPOSED";
		case SDL_WINDOWEVENT_MOVED: return "SDL_WINDOWEVENT_MOVED";
		case SDL_WINDOWEVENT_RESIZED: return "SDL_WINDOWEVENT_RESIZED";
		case SDL_WINDOWEVENT_SIZE_CHANGED: return "SDL_WINDOWEVENT_SIZE_CHANGED";
		case SDL_WINDOWEVENT_MINIMIZED: return "SDL_WINDOWEVENT_MINIMIZED";
		case SDL_WINDOWEVENT_MAXIMIZED: return "SDL_WINDOWEVENT_MAXIMIZED";
		case SDL_WINDOWEVENT_RESTORED: return "SDL_WINDOWEVENT_RESTORED";
		case SDL_WINDOWEVENT_ENTER: return "SDL_WINDOWEVENT_ENTER";
		case SDL_WINDOWEVENT_LEAVE: return "SDL_WINDOWEVENT_LEAVE";
		case SDL_WINDOWEVENT_FOCUS_GAINED: return "SDL_WINDOWEVENT_FOCUS_GAINED";
		case SDL_WINDOWEVENT_FOCUS_LOST: return "SDL_WINDOWEVENT_FOCUS_LOST";
		case SDL_WINDOWEVENT_CLOSE: return "SDL_WINDOWEVENT_CLOSE";
		case SDL_WINDOWEVENT_TAKE_FOCUS: return "SDL_WINDOWEVENT_TAKE_FOCUS";
		case SDL_WINDOWEVENT_HIT_TEST: return "SDL_WINDOWEVENT_HIT_TEST";
		default: return "<invalid>";
	}
}

void HandleWindowEvent(const SDL_WindowEvent& windowEvent)
{
	switch (windowEvent.event)
	{
	case SDL_WINDOWEVENT_RESIZED:
	case SDL_WINDOWEVENT_SIZE_CHANGED:
	{
		Poly::ScreenSize screenSize;
		screenSize.Width = windowEvent.data1;
		screenSize.Height = windowEvent.data2;
		Poly::gEngine->ResizeScreen(screenSize);
		break;
	}
	case SDL_WINDOWEVENT_LEAVE:
	case SDL_WINDOWEVENT_FOCUS_LOST:
		UpdateMouseState(eMouseStateChange::WINDOW_LEAVE);
		break;
	case SDL_WINDOWEVENT_ENTER:
	case SDL_WINDOWEVENT_FOCUS_GAINED:	
		UpdateMouseState(eMouseStateChange::WINDOW_ENTER);
		break;
	default:
		Poly::gConsole.LogDebug("Not handled SDL window event, code: {}", GetWindowEventName(windowEvent));
		break;
	}
}


void UpdateMouseState(eMouseStateChange change)
{
	if (change == eMouseStateChange::BUTTON_CLICK)
	{
		SDL_ShowCursor(Poly::gEngine->ShouldMouseBeVisible());
		SDL_CaptureMouse(Poly::gEngine->ShouldCaptureMouse() ? SDL_bool::SDL_TRUE : SDL_bool::SDL_FALSE);
		SDL_SetRelativeMouseMode(Poly::gEngine->ShouldCaptureMouse() ? SDL_bool::SDL_TRUE : SDL_bool::SDL_FALSE);
	}

	if(change == eMouseStateChange::WINDOW_LEAVE || change == eMouseStateChange::WINDOW_ENTER)
	{
		SDL_ShowCursor(1);
		SDL_CaptureMouse(SDL_bool::SDL_FALSE);
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
}