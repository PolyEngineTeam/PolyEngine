#include "Engine.hpp"

using namespace Poly;

Engine::Engine(IGame* game) : Game(game) 
{
	Game->RegisterEngine(this);
	Renderer = CreateRenderingContext();
	InputEventsQueue = new InputQueue;
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

void Engine::RegisterUpdatePhase(const PhaseUpdateFunction& phaseFunction, eUpdatePhaseOrder order)
{
	HEAVY_ASSERTE(order != eUpdatePhaseOrder::_COUNT, "_COUNT enum value passed to RegisterUpdatePhase(), which is an invalid value");
	Dynarray<PhaseUpdateFunction>& UpdatePhases = GameUpdatePhases[static_cast<int>(order)];
	for (auto& iter : UpdatePhases)
	{
		HEAVY_ASSERTE(iter.target<PhaseUpdateFunction>() != phaseFunction.target<PhaseUpdateFunction>(), "Failed at RegisterUpdatePhase() - passed function is already registered as a phase in this update stage.");
	}
	UpdatePhases.PushBack(phaseFunction);
}

// temporary include, remove this ASAP
#include <gl/glew.h>

void Engine::Update(float dt)
{
	UpdatePhases(eUpdatePhaseOrder::PREUPDATE);

	UpdatePhases(eUpdatePhaseOrder::UPDATE);

	UpdatePhases(eUpdatePhaseOrder::POSTUPDATE);

	// quite stupid test for input :P
	while(InputEventsQueue->Size() > 0){
		if(InputEventsQueue->Front().Type == eInputEventType::KEYDOWN)
			gConsole.LogDebug("KEYDOWN");
		else if(InputEventsQueue->Front().Type == eInputEventType::KEYUP)
			gConsole.LogDebug("KEYUP");
		else if(InputEventsQueue->Front().Type == eInputEventType::MOUSEMOVE)
			gConsole.LogDebug("MOUSEMOVE");
		else if(InputEventsQueue->Front().Type == eInputEventType::WHEELMOVE)
			gConsole.LogDebug("WHEELMOVE");
		InputEventsQueue->Pop();
	}

	// very simple temporary loop, this should be moved somwhere else
	glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer->EndFrame();
}