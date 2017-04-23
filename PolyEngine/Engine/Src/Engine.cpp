#include "Engine.hpp"

#include "TransformComponent.hpp"
#include "CameraSystem.hpp"
#include "CameraComponent.hpp"
#include "World.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Engine::Engine(IGame* game) : Game(game)
{
	BaseWorld = new World(this);
	Game->RegisterEngine(this);
	Renderer = CreateRenderingContext();
}

//------------------------------------------------------------------------------
Poly::Engine::~Engine()
{
	delete BaseWorld;
}

//------------------------------------------------------------------------------
bool Engine::Init(const IRenderingContextParams* context) 
{
	// Engine Components
	REGISTER_COMPONENT(this, TransformComponent, (size_t)eEngineComponents::TRANSFORM);
	REGISTER_COMPONENT(this, BaseCameraComponent, (size_t)eEngineComponents::BASE_CAMERA);

	// Engine update phases
	RegisterUpdatePhase(CameraSystem::CameraUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	
	if (!Renderer->Init(context))
		return false;
	Game->Init();
	return true;
}

//------------------------------------------------------------------------------
void Engine::Deinit()
{
	Renderer->Deinit();
	delete Renderer;
	Renderer = nullptr;
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void Engine::Update(float dt)
{
	// quite stupid test for input :P
	while(InputEventsQueue.Size() > 0){
		if(InputEventsQueue.Front().Type == eEventType::KEYDOWN)
			gConsole.LogDebug("Keydown: {}", (unsigned)InputEventsQueue.Front().Key);
		else if(InputEventsQueue.Front().Type == eEventType::KEYUP)
			gConsole.LogDebug("Keyup: {}", (unsigned)InputEventsQueue.Front().Key);
		else if(InputEventsQueue.Front().Type == eEventType::MOUSEMOVE)
			gConsole.LogDebug("Mousemove: {}", InputEventsQueue.Front().Pos);
		else if(InputEventsQueue.Front().Type == eEventType::WHEELMOVE)
			gConsole.LogDebug("Wheelmoove: {}", InputEventsQueue.Front().Pos);
		InputEventsQueue.Pop();
	}

	UpdatePhases(eUpdatePhaseOrder::PREUPDATE);
	UpdatePhases(eUpdatePhaseOrder::UPDATE);
	UpdatePhases(eUpdatePhaseOrder::POSTUPDATE);

	// very simple temporary loop, this should be moved somwhere else
	glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer->EndFrame();
}