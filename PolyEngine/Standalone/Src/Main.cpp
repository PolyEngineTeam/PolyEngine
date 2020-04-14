// include the basic windows header file
#include <SDL.h>

namespace pe{}
using namespace pe;

#include <pe/engine/Engine.hpp>
#include <pe/api/rendering/IRenderingDevice.hpp>
#include <pe/api/input/KeyBindings.hpp>
#include <pe/api/input/OutputQueue.hpp>
#include <pe/api/input/InputQueue.hpp>
#include <pe/core/utils/LibraryLoader.hpp>
#include <pe/engine/config/AssetsPathConfig.hpp>
#include <pe/core/utils/OutputStream.hpp>
#include <pe/core/utils/Logger.hpp>
#include <pe/core/math/Vector2i.hpp>
#include <pe/api/app/App.hpp>
#include <pe/api/app/IAppBackend.hpp>

extern "C"
{
	using CreateRenderingDeviceFunc = pe::api::rendering::IRenderingDevice* (SDL_Window*, const ::pe::core::math::Vector2i&);
	using CreateGameFunc = pe::api::IGame* (void);
}

class AppBackend : public  pe::api::app::IAppBackend
{

};

int main(int argc, char * args[]);

void HandleWindowEvent(const SDL_WindowEvent& windowEvent);

enum eMouseStateChange {
	WINDOW_ENTER,
	WINDOW_LEAVE,
	BUTTON_CLICK,
	_COUNT
};

void UpdateMouseState(eMouseStateChange change);

class FileAndCoutStream : public ::pe::core::utils::FileOutputStream
{
public:
	FileAndCoutStream(const char* name) : ::pe::core::utils::FileOutputStream(name) {}
	void Append(const char* data) override {
		std::cout << data;
		std::cout.flush();
		::pe::core::utils::FileOutputStream::Append(data);
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

SDL_SystemCursor GetCursorType(pe::api::input::eMouseCursorType cursorType);

int main(int argc, char* args[])
{
	for(int i=0; i<argc; ++i)
	{
		core::utils::gConsole.LogDebug("Runtime arg[{}] = {}", i, args[i]);
	}

	core::utils::gConsole.RegisterStream<FileAndCoutStream>("console.log");
	UNUSED(argc);
	UNUSED(args);
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		ASSERTE(false, "SDL initialization failed!");
		return 1;
	}
	core::utils::gConsole.LogDebug("SDL initialized.");

	// Initial screen size
	pe::core::math::Vector2i screenSize;
	screenSize.X = 1280;
	screenSize.Y = 720;

	// Main standalone window
	SDL_Window* window = SDL_CreateWindow(
		"PolyEngine standalone",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		screenSize.X,
		screenSize.Y,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	ASSERTE(window, "Failed to create standalone window!");
	core::utils::gConsole.LogDebug("Window created.");

	std::unique_ptr<pe::api::app::App> application = std::make_unique<pe::api::app::App>(std::make_unique<AppBackend>());
	
	application->registerEngine(std::make_unique<pe::engine::Engine>());
	core::utils::gConsole.LogDebug("Engine created.");

	// Load rendering device library
	core::storage::String p = pe::engine::config::gAssetsPathConfig.GetRenderingDeviceLibPath();
	auto loadRenderingDevice = ::pe::core::utils::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>(p.GetCStr(), "PolyCreateRenderingDevice");
	if (!loadRenderingDevice.FunctionValid())
		return 1;
	core::utils::gConsole.LogDebug("Library libRenderingDevice loaded.");

	// Load game library
	auto loadGame = ::pe::core::utils::LoadFunctionFromSharedLibrary<CreateGameFunc>(pe::engine::config::gAssetsPathConfig.GetGameLibPath().GetCStr(), "CreateGame");
	if (!loadGame.FunctionValid())
		return 1;
	core::utils::gConsole.LogDebug("Library libGame loaded.");
		
	application->registerGame(std::unique_ptr<pe::api::IGame>(loadGame()));
	core::utils::gConsole.LogDebug("Game created");

	application->registerRenderingDevice(std::unique_ptr<pe::api::rendering::IRenderingDevice>(loadRenderingDevice(window, screenSize)));
	core::utils::gConsole.LogDebug("Device created.");

	//Engine->init(std::move(game), std::move(device));
	application->getEngine()->startGame();
	core::utils::gConsole.LogDebug("Engine initialization and handshake successfull. Starting main loop...");

	pe::engine::Engine* tmpEnginePtr = static_cast<pe::engine::Engine*>(application->getEngine());
	tmpEnginePtr->SetClipboardTextFunction = HandleSetClipboardText;
	tmpEnginePtr->GetClipboardTextFunction = HandleGetClipboardText;

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
				application->keyDown(static_cast<pe::api::input::eKey>(event.key.keysym.scancode));
				break;
			case SDL_KEYUP:
				application->keyUp(static_cast<pe::api::input::eKey>(event.key.keysym.scancode));
				break;
			case SDL_TEXTINPUT:
				application->addCharacterUTF8(event.text.text);
				break;
			case SDL_MOUSEMOTION:
			{
				application->updateMouseMove(::pe::core::math::Vector2i(event.motion.xrel, event.motion.yrel));
				application->updateMousePos(::pe::core::math::Vector2i(event.motion.x, event.motion.y));
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
				application->mouseButtonDown(static_cast<pe::api::input::eMouseButton>(event.button.button));
				UpdateMouseState(eMouseStateChange::BUTTON_CLICK);
				break;
			case SDL_MOUSEBUTTONUP:
				application->mouseButtonUp(static_cast<pe::api::input::eMouseButton>(event.button.button));
				break;
			case SDL_MOUSEWHEEL:
				// Not sure if this is correct.
				application->updateWheelPos(::pe::core::math::Vector2i(event.wheel.x, event.wheel.y));
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				application->controllerButtonDown(event.cbutton.which, static_cast<pe::api::input::eControllerButton>(event.cbutton.button));
				break;
			case SDL_CONTROLLERBUTTONUP:
				application->controllerButtonUp(event.cbutton.which, static_cast<pe::api::input::eControllerButton>(event.cbutton.button));
				break;
			case SDL_CONTROLLERAXISMOTION:
				application->controllerAxisMotion(event.caxis.which, static_cast<pe::api::input::eControllerAxis>(event.caxis.axis), event.caxis.value);
				break;
			case SDL_CONTROLLERDEVICEADDED:
			{
				SDL_GameController* controller = SDL_GameControllerOpen(event.cdevice.which);
				SDL_Joystick* joystickPtr = SDL_GameControllerGetJoystick(controller);
				i32 joystickID = SDL_JoystickInstanceID(joystickPtr);
				application->addController(joystickID);
				break;
			}
			case SDL_CONTROLLERDEVICEREMOVED:
			{
				SDL_GameController* controller = SDL_GameControllerFromInstanceID(event.cdevice.which);
				SDL_GameControllerClose(controller);
				application->removeController(event.cdevice.which);
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
		application->getEngine()->update(std::chrono::duration<double>(0.16));

		pe::api::input::OutputQueue& OutputEventsQueue = application->getOutputQueue();
		while (!OutputEventsQueue.isEmpty())
		{
			pe::api::input::OutputEvent& ev = OutputEventsQueue.front();
			switch (ev.type)
			{
			case pe::api::input::eOutputEventType::MOUSEPOS:
				SDL_WarpMouseInWindow(window, ev.pos.X, ev.pos.Y);
				break;
			case pe::api::input::eOutputEventType::CURSORSET:
				SDL_SetCursor(mouseCursors[(int)GetCursorType(ev.cursorType)]);
				break;
			case pe::api::input::eOutputEventType::CURSORSHOW:
				SDL_ShowCursor(SDL_TRUE);
				break;
			case pe::api::input::eOutputEventType::CURSORHIDE:
				SDL_ShowCursor(SDL_FALSE);
				break;
			case pe::api::input::eOutputEventType::_COUNT:
				HEAVY_ASSERTE(false, "_COUNT enum value passed to InputEventQueue::push(), which is an invalid value");
				break;
			}
			OutputEventsQueue.popFront();
		}

		quitRequested = quitRequested || application->getEngine()->isQuitRequested();
	}
	core::utils::gConsole.LogDebug("Closing main loop...");
	
	application.reset();

	// Destroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();
	core::utils::gConsole.LogDebug("Exiting...");

	// Clean managers, otherwise their constructors crash due to messed deinitialization order across shared libraries
	pe::api::ecs::ComponentManager::Get().Clear();
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
		::pe::core::math::Vector2i screenSize;
		screenSize.X = windowEvent.data1;
		screenSize.Y = windowEvent.data2;
		//Poly::gEngine->ResizeScreen(screenSize);
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
		core::utils::gConsole.LogDebug("Not handled SDL window event, code: {}", GetWindowEventName(windowEvent));
		break;
	}
}

void UpdateMouseState(eMouseStateChange change)
{
	// if (change == eMouseStateChange::BUTTON_CLICK)
	// {
	// 	SDL_ShowCursor(Poly::gEngine->ShouldMouseBeVisible());
	// 	SDL_CaptureMouse(Poly::gEngine->ShouldCaptureMouse() ? SDL_bool::SDL_TRUE : SDL_bool::SDL_FALSE);
	// 	SDL_SetRelativeMouseMode(Poly::gEngine->ShouldCaptureMouse() ? SDL_bool::SDL_TRUE : SDL_bool::SDL_FALSE);
	// }

	// if(change == eMouseStateChange::WINDOW_LEAVE || change == eMouseStateChange::WINDOW_ENTER)
	// {
	// 	SDL_ShowCursor(1);
	// 	SDL_CaptureMouse(SDL_bool::SDL_FALSE);
	// 	SDL_SetRelativeMouseMode(SDL_FALSE);
	// }
}

SDL_SystemCursor GetCursorType(pe::api::input::eMouseCursorType cursorType)
{
	switch (cursorType)
	{
		case pe::api::input::eMouseCursorType::ARROW:			return SDL_SYSTEM_CURSOR_ARROW;
		case pe::api::input::eMouseCursorType::TEXTINPUT:		return SDL_SYSTEM_CURSOR_IBEAM;
		case pe::api::input::eMouseCursorType::RESIZEALL:		return SDL_SYSTEM_CURSOR_SIZEALL;
		case pe::api::input::eMouseCursorType::RESIZENS:		return SDL_SYSTEM_CURSOR_SIZENS;
		case pe::api::input::eMouseCursorType::RESIZEEW:		return SDL_SYSTEM_CURSOR_SIZEWE;
		case pe::api::input::eMouseCursorType::RESIZENESW:	return SDL_SYSTEM_CURSOR_SIZENESW;
		case pe::api::input::eMouseCursorType::RESIZENWSE:	return SDL_SYSTEM_CURSOR_SIZENWSE;
		case pe::api::input::eMouseCursorType::HAND:			return SDL_SYSTEM_CURSOR_HAND;
		default: return SDL_SYSTEM_CURSOR_ARROW;
	}
}