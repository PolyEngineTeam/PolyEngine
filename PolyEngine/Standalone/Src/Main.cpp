// include the basic windows header file
#include <SDL.h>

#include <Engine.hpp>
#include <LibraryLoader.hpp>


using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (SDL_Window*, const Poly::ScreenSize&);
using CreateGameFunc = Poly::IGame* (void);

void HandleWindowEvent(const SDL_WindowEvent& windowEvent);

int main(int argc, char* args[])
{
	UNUSED(argc);
	UNUSED(args);
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ASSERTE(false, "SDL initialization failed!");
		return 1;
	}
	Poly::gConsole.LogDebug("SDL initialized.");
	
	// Initial screen size
	Poly::ScreenSize screenSize;
	screenSize.Width = 800;
	screenSize.Height = 600;

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

	// Load rendering device library
	auto loadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libRenderingDevice", "PolyCreateRenderingDevice");
	// Try to load library with different name
    if(!loadRenderingDevice.FunctionValid())
        loadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libpolyrenderingdevice", "PolyCreateRenderingDevice");
    if (!loadRenderingDevice.FunctionValid())
		return 1;
	Poly::gConsole.LogDebug("Library libRenderingDevice loaded.");

	// Load game library
	auto loadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libGame", "CreateGame");
    // Try to load library with different name
    if (!loadGame.FunctionValid())
        loadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libgame", "CreateGame");
    if (!loadGame.FunctionValid())
		return 1;
	Poly::gConsole.LogDebug("Library libGame loaded.");
	
	{ // Start engine scope
		Poly::Engine Engine;
		Poly::gConsole.LogDebug("Engine created.");
		
		std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(loadGame());
		Poly::gConsole.LogDebug("Game created");

		std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(loadRenderingDevice(window, screenSize));
		Poly::gConsole.LogDebug("Device created.");

		Engine.Init(std::move(game), std::move(device));
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
				// TODO add controller support
				case SDL_QUIT:
					quitRequested = true;
					break;
				case SDL_KEYDOWN:
					Engine.KeyDown(static_cast<Poly::eKey>(event.key.keysym.scancode));
					break;
				case SDL_KEYUP:
					Engine.KeyUp(static_cast<Poly::eKey>(event.key.keysym.scancode));
					break;
				case SDL_MOUSEMOTION:
					Engine.UpdateMousePos(Poly::Vector2i(event.motion.x, event.motion.y));
					break;
				case SDL_MOUSEBUTTONDOWN:
                    Engine.MouseButtonDown(static_cast<Poly::eMouseButton>(event.button.button));
                    break;
				case SDL_MOUSEBUTTONUP:
                    Engine.MouseButtonUp(static_cast<Poly::eMouseButton>(event.button.button));
					break;
				case SDL_MOUSEWHEEL:
					// Not sure if this is correct.
					Engine.UpdateWheelPos(Poly::Vector2i(event.wheel.x, event.wheel.y));
					break;
				case SDL_WINDOWEVENT:
					HandleWindowEvent(event.window);
					break;
				default:
					break;
				}
			}

			// Engine loop
			Engine.Update();
			quitRequested = quitRequested || Engine.IsQuitRequested();
		}
		Poly::gConsole.LogDebug("Closing main loop...");
	} // End engine scope
	
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
	default:
		Poly::gConsole.LogDebug("Not handled SDL window event, code: {}", GetWindowEventName(windowEvent));
		break;
	}
}