// include the basic windows header file
#include <SDL.h>

#include <Engine.hpp>
#include <TimeSystem.hpp>
#include <LibraryLoader.hpp>


using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (SDL_Window*, const Poly::ScreenSize&);
using CreateGameFunc = Poly::IGame* (void);

void HandleWindowEvent(SDL_WindowEvent windowEvent);

int main(int argc, char* args[])
{
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
		SDL_WINDOW_OPENGL
	);

	ASSERTE(window, "Failed to create standalone window!");
	Poly::gConsole.LogDebug("Window created.");

	// Load rendering device library
	auto loadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libRenderingDevice", "PolyCreateRenderingDevice");
	if (!loadRenderingDevice.FunctionValid())
		return 1;
	Poly::gConsole.LogDebug("Library libRenderingDevice loaded.");

	// Load game library
	auto loadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libGame", "CreateGame");
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
					Engine.KeyDown(static_cast<Poly::eKey>(event.key.keysym.sym));
					break;
				case SDL_KEYUP:
					Engine.KeyUp(static_cast<Poly::eKey>(event.key.keysym.sym));
					break;
				case SDL_MOUSEMOTION:
					Engine.UpdateMousePos(Poly::Vector(event.motion.x, event.motion.y, 0));
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					Poly::gConsole.LogDebug("Not handled SDL event, code: {}", event.type);
					break;
				case SDL_MOUSEWHEEL:
					// Not sure if this is correct.
					Engine.UpdateWheelPos(Poly::Vector(event.wheel.x, event.wheel.y, 0));
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

void HandleWindowEvent(SDL_WindowEvent windowEvent)
{
	switch (windowEvent.type)
	{
	default:
		Poly::gConsole.LogDebug("Not handled SDL window event, code: {}", windowEvent.type);
		break;
	}
}