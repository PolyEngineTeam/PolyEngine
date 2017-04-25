#include "Engine.hpp"


#include "World.hpp"

#include "TransformComponent.hpp"
#include "CameraComponent.hpp"
#include "MeshRenderingComponent.hpp"

#include "CameraSystem.hpp"
#include "RenderingSystem.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Engine::Engine(IGame* game) : Game(game)
{
	BaseWorld = new World(this);
	Game->RegisterEngine(this);
	Renderer = CreateRenderingContext();
	InputEventsQueue = new InputQueue;
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
	REGISTER_COMPONENT(this, TransformComponent, eEngineComponents::TRANSFORM);
	REGISTER_COMPONENT(this, BaseCameraComponent, eEngineComponents::BASE_CAMERA);
	REGISTER_COMPONENT(this, MeshRenderingComponent, eEngineComponents::MESH_RENDERING);

	// Engine update phases
	RegisterUpdatePhase(CameraSystem::CameraUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(RenderingSystem::RenderingPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(InputSystem::InputPhase, eUpdatePhaseOrder::PREUPDATE);
	
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
	UpdatePhases.PushBack(phaseFunction);
}

//------------------------------------------------------------------------------
void Engine::Update(float dt)
{
	UpdatePhases(eUpdatePhaseOrder::PREUPDATE);
	UpdatePhases(eUpdatePhaseOrder::UPDATE);
	UpdatePhases(eUpdatePhaseOrder::POSTUPDATE);
}