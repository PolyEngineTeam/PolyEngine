#include "Engine.hpp"

using namespace Poly;

Engine::Engine(IGame* game) : Game(game) 
{
	Game->RegisterEngine(this);
	Renderer = CreateRenderingContext();
}

bool Engine::Init(const IRenderingContextParams* context) 
{
	if (!Renderer->Init(context))
		return false;
	Game->Init();
	return true;
}

void Engine::Deinit()
{
	Renderer->Deinit();
	delete Renderer;
	Renderer = nullptr;
}

void Engine::RegisterPreupdatePhase(PhaseUpdateFunction phaseFunction)
{
	for (auto& iter : GamePreupdatePhases)
	{
		if (iter.target<PhaseUpdateFunction>() == phaseFunction.target<PhaseUpdateFunction>())
			return;
	}
	GamePreupdatePhases.PushBack(phaseFunction);
}

void Engine::RegisterUpdatePhase(PhaseUpdateFunction phaseFunction)
{
	for (auto& iter : GameUpdatePhases)
	{
		if (iter.target<PhaseUpdateFunction>() == phaseFunction.target<PhaseUpdateFunction>())
			return;
	}
	GameUpdatePhases.PushBack(phaseFunction);
}

void Engine::RegisterPostupdatePhase(PhaseUpdateFunction phaseFunction)
{
	for (auto& iter : GamePostupdatePhases)
	{
		if (iter.target<PhaseUpdateFunction>() == phaseFunction.target<PhaseUpdateFunction>())
			return;
	}
	GamePostupdatePhases.PushBack(phaseFunction);
}

// temporary include, remove this ASAP
#include <gl/glew.h>

void Engine::Update(float dt)
{
	PreupdatePhases();

	UpdatePhases();

	PostupdatePhases();

	// quite stupid test for input :P
	while(InputEventsQueue.Size() > 0){
		if(InputEventsQueue.Front().Type == eEventType::KEYDOWN)
			gConsole.LogDebug("KEYDOWN");
		else if(InputEventsQueue.Front().Type == eEventType::KEYUP)
			gConsole.LogDebug("KEYUP");
		else if(InputEventsQueue.Front().Type == eEventType::MOUSEMOVE)
			gConsole.LogDebug("MOUSEMOVE");
		else if(InputEventsQueue.Front().Type == eEventType::WHEELMOVE)
			gConsole.LogDebug("WHEELMOVE");
		InputEventsQueue.Pop();
	}

	// very simple temporary loop, this should be moved somwhere else
	glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer->EndFrame();
}

void Engine::PreupdatePhases()
{
	for (auto& update : GamePreupdatePhases)
		update(&GetWorld());
}

void Engine::UpdatePhases()
{
	for (auto& update : GameUpdatePhases)
		update(&GetWorld());
}

void Engine::PostupdatePhases()
{
	for (auto& update : GamePostupdatePhases)
		update(&GetWorld());
}