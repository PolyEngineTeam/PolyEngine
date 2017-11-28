// include the basic windows header file
#include <SDL.h>

#include <Engine.hpp>
#include <TimeSystem.hpp>
#include <LibraryLoader.hpp>


using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (SDL_Window*, const Poly::ScreenSize&);
using CreateGameFunc = Poly::IGame* (void);

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ASSERTE(false, "SDL initialization failed!");
		return 1;
	}
	
	//The window we'll be rendering to
	SDL_Window* window = SDL_CreateWindow(
		"PolyEngine standalone",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		600,
		SDL_WINDOW_OPENGL
	);

	auto loadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>("libRenderingDevice", "PolyCreateRenderingDevice");
	if (!loadRenderingDevice.FunctionValid()) { return 1; }
	auto loadGame = Poly::LoadFunctionFromSharedLibrary<CreateGameFunc>("libGame", "CreateGame");
	if (!loadGame.FunctionValid()) { return 1; }

	{
		Poly::Engine Engine;
		
		std::unique_ptr<Poly::IGame> game = std::unique_ptr<Poly::IGame>(loadGame());

		Poly::ScreenSize size;
		size.Width = 800;
		size.Height = 600;
		std::unique_ptr<Poly::IRenderingDevice> device = std::unique_ptr<Poly::IRenderingDevice>(loadRenderingDevice(window, size));

		Engine.Init(std::move(game), std::move(device));
		Poly::gConsole.LogDebug("Engine loaded successfully");


		bool quitRequested = false;
		while (!quitRequested)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
					quitRequested = true;

				if (event.type == SDL_KEYDOWN)
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						quitRequested = true;
						break;
					default:
						break;
					}
				}
			}

			// engine loop
			Engine.Update();
		}

	}
	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}