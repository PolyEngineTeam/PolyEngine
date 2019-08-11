// include the basic windows header file
#include <SDL.h>

#include <Engine.hpp>
#include <Rendering/IRenderingDevice.hpp>
#include <pe/core/utils/LibraryLoader.hpp>
#include <Configs/AssetsPathConfig.hpp>
#include <pe/core/utils/OutputStream.hpp>
#include <pe/core/utils/Logger.hpp>
#include <pe/core/math/Vector2i.hpp>

extern "C"
{
	using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (SDL_Window*, const Poly::ScreenSize&);
	using CreateGameFunc = Poly::IGame* (void);
}

int main(int argc, char * args[]);

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

static char* g_ClipboardTextData = NULL;
static const char* HandleGetClipboardText(void*)
{
	if (g_ClipboardTextData)
		SDL_free(g_ClipboardTextData);
	g_ClipboardTextData = SDL_GetClipboardText();
	return g_ClipboardTextData;
}
static void HandleSetClipboardText(void*, const char* text)
{
	SDL_SetClipboardText(text);
}

SDL_SystemCursor GetCursorType(Poly::eMouseCursorType cursorType);

int main(int argc, char* args[])
{
	for(int i=0; i<argc; ++i)
	{
		Poly::gConsole.LogDebug("Runtime arg[{}] = {}", i, args[i]);
	}

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
	screenSize.Width = 1280;
	screenSize.Height = 720;

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
	Poly::String p = Poly::gAssetsPathConfig.GetRenderingDeviceLibPath();
	auto loadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>(p.GetCStr(), "PolyCreateRenderingDevice");
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
	Engine->StartGame();
	Poly::gConsole.LogDebug("Engine initialization and handshake successfull. Starting main loop...");

	Engine->SetClipboardTextFunction = HandleSetClipboardText;
	Engine->GetClipboardTextFunction = HandleGetClipboardText;

	SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

	SDL_Cursor* mouseCursors[(int)SDL_NUM_SYSTEM_CURSORS] = { 0 };
	for (int i = SDL_NUM_SYSTEM_CURSORS - 1; i >= 0; --i)
		mouseCursors[i] = SDL_CreateSystemCursor((SDL_SystemCursor)i);

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
			case SDL_TEXTINPUT:
				Engine->AddCharacterUTF8(event.text.text);
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

		Poly::OutputQueue& OutputEventsQueue = Engine->GetOutputQueue();
		while (!OutputEventsQueue.IsEmpty())
		{
			Poly::OutputEvent& ev = OutputEventsQueue.Front();
			switch (ev.Type)
			{
			case Poly::eOutputEventType::MOUSEPOS:
				SDL_WarpMouseInWindow(window, ev.Pos.X, ev.Pos.Y);
				break;
			case Poly::eOutputEventType::CURSORSET:
				SDL_SetCursor(mouseCursors[(int)GetCursorType(ev.CursorType)]);
				break;
			case Poly::eOutputEventType::CURSORSHOW:
				SDL_ShowCursor(SDL_TRUE);
				break;
			case Poly::eOutputEventType::CURSORHIDE:
				SDL_ShowCursor(SDL_FALSE);
				break;
			case Poly::eOutputEventType::_COUNT:
				HEAVY_ASSERTE(false, "_COUNT enum value passed to InputEventQueue::Push(), which is an invalid value");
				break;
			}
			OutputEventsQueue.PopFront();
		}

		quitRequested = quitRequested || Engine->IsQuitRequested();
	}
	Poly::gConsole.LogDebug("Closing main loop...");
	
	Engine.reset();

	// Destroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();
	Poly::gConsole.LogDebug("Exiting...");

	// Clean managers, otherwise their constructors crash due to messed deinitialization order across shared libraries
	Poly::ComponentManager::Get().Clear();
	Poly::RTTI::Impl::TypeManager::Get().Clear();

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

SDL_SystemCursor GetCursorType(Poly::eMouseCursorType cursorType)
{
	switch (cursorType)
	{
		case Poly::eMouseCursorType::ARROW:			return SDL_SYSTEM_CURSOR_ARROW;
		case Poly::eMouseCursorType::TEXTINPUT:		return SDL_SYSTEM_CURSOR_IBEAM;
		case Poly::eMouseCursorType::RESIZEALL:		return SDL_SYSTEM_CURSOR_SIZEALL;
		case Poly::eMouseCursorType::RESIZENS:		return SDL_SYSTEM_CURSOR_SIZENS;
		case Poly::eMouseCursorType::RESIZEEW:		return SDL_SYSTEM_CURSOR_SIZEWE;
		case Poly::eMouseCursorType::RESIZENESW:	return SDL_SYSTEM_CURSOR_SIZENESW;
		case Poly::eMouseCursorType::RESIZENWSE:	return SDL_SYSTEM_CURSOR_SIZENWSE;
		case Poly::eMouseCursorType::HAND:			return SDL_SYSTEM_CURSOR_HAND;
		default: return SDL_SYSTEM_CURSOR_ARROW;
	}
}