#include "Engine.hpp"

using namespace Poly;

Engine::Engine(IGame* game) : Game(game) {
	Game->RegisterEngine(this);
	Renderer = CreateRenderingContext();
}

bool Engine::Init(const IRenderingContextParams* context) {
	if (!Renderer->Init(context))
		return false;
	Game->Init();
	return true;
}

void Poly::Engine::Deinit()
{
	Renderer->Deinit();
	delete Renderer;
	Renderer = nullptr;
}

// temporary include, remove this ASAP
#include <gl/glew.h>

void Engine::Update(float dt) {
	Game->Update(dt);

	// quite stupid test for input :P
	for(int i = 0; i < InputBase.Size(); i++){
		//gConsole.LogDebug("Event");
		if(InputBase.Front().Type == eEventType::KEYDOWN)
			gConsole.LogDebug("KEYDOWN");
		if(InputBase.Front().Type == eEventType::KEYUP)
			gConsole.LogDebug("KEYUP");
		if(InputBase.Front().Type == eEventType::MOUSEMOVE)
			gConsole.LogDebug("MOUSEMOVE");
		if(InputBase.Front().Type == eEventType::WHEELMOVE)
			gConsole.LogDebug("WHEELMOVE");
		InputBase.Pop();
	}

	// very simple temporary loop, this should be moved somwhere else
	glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer->EndFrame();
}
