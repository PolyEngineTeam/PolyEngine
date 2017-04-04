#include "Engine.hpp"

using namespace Poly;

Engine::Engine(IGame* game) : Game(game) {
	Game->RegisterEngine(this);
	Renderer = CreateRenderer();
}
void Engine::Init(const IRenderingContext* context) {
	Renderer->Init(context);
	Game->Init();
}

void Poly::Engine::Deinit()
{
	Renderer->Deinit();
	delete Renderer;
	Renderer = nullptr;
}

void Engine::Update(float dt) {
	Game->Update(dt);
	Renderer->Update(dt);
}
