#include "EnginePCH.hpp"
#include "HeavyTaskSystem.hpp"

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
	RegisterComponent<TransformComponent>((size_t) eEngineComponents::TRANSFORM);
	RegisterComponent<CameraComponent>((size_t) eEngineComponents::BASE_CAMERA);
	RegisterComponent<MeshRenderingComponent>((size_t) eEngineComponents::MESH_RENDERING);
	RegisterComponent<FreeFloatMovementComponent>((size_t)eEngineComponents::FREE_FLOAT_MOVEMENT);

	// Engine update phases
	RegisterUpdatePhase(TimeSystem::TimeUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(InputSystem::InputPhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(MovementSystem::MovementUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(CameraSystem::CameraUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(RenderingSystem::RenderingPhase, eUpdatePhaseOrder::POSTUPDATE);

	if (!Renderer->Init(context))
		return false;

	RegisterUpdatePhase(HeavyTaskSystem::HeavyTaskPhase, eUpdatePhaseOrder::POSTUPDATE);
	Game->Init();

	return true;
}

//------------------------------------------------------------------------------
void Engine::Deinit()
{
	Game->Deinit();
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
void Engine::Update()
{
	UpdatePhases(eUpdatePhaseOrder::PREUPDATE);
	UpdatePhases(eUpdatePhaseOrder::UPDATE);
	UpdatePhases(eUpdatePhaseOrder::POSTUPDATE);
}

//------------------------------------------------------------------------------
void Poly::Engine::ResizeScreen(const ScreenSize & size)
{
	gConsole.LogDebug("Screen resize: {} {}", size.Width, size.Height);
	GetRenderingContext()->Resize(size);
}
