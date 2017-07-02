#include "EnginePCH.hpp"


using namespace Poly;

Engine* Poly::gEngine = nullptr;

//------------------------------------------------------------------------------
Engine::Engine(std::unique_ptr<IGame> game, std::unique_ptr<IRenderingDevice> device) 
	: Game(std::move(game)), RenderingDevice(std::move(device))
{
	ASSERTE(gEngine == nullptr, "Creating engine twice?");
	gEngine = this;
	BaseWorld = std::make_unique<World>();
	Game->RegisterEngine(this);

	// Engine Components
	RegisterComponent<TransformComponent>((size_t)eEngineComponents::TRANSFORM);
	RegisterComponent<CameraComponent>((size_t)eEngineComponents::BASE_CAMERA);
	RegisterComponent<MeshRenderingComponent>((size_t)eEngineComponents::MESH_RENDERING);
	RegisterComponent<FreeFloatMovementComponent>((size_t)eEngineComponents::FREE_FLOAT_MOVEMENT);
	RegisterComponent<ScreenSpaceTextComponent>((size_t)eEngineComponents::SCREEN_SPACE_TEXT);

	// Engine World Components
	RegisterWorldComponent<InputWorldComponent>((size_t)eEngineWorldComponents::INPUT);
	RegisterWorldComponent<ViewportWorldComponent>((size_t)eEngineWorldComponents::VIEWPORT);
	RegisterWorldComponent<TimeWorldComponent>((size_t)eEngineWorldComponents::TIME);
	RegisterWorldComponent<DebugWorldComponent>((size_t)eEngineWorldComponents::DEBUG);
	RegisterWorldComponent<DeferredTaskWorldComponent>((size_t)eEngineWorldComponents::DEFERRED_TASK);

	// Add WorldComponents
	DeferredTaskSystem::AddWorldComponentImmediate<InputWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<ViewportWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<TimeWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<DebugWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<DeferredTaskWorldComponent>(BaseWorld.get());

	// Engine update phases
	RegisterUpdatePhase(TimeSystem::TimeUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(InputSystem::InputPhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(MovementSystem::MovementUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(CameraSystem::CameraUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(RenderingSystem::RenderingPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(DeferredTaskSystem::DeferredTaskPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(FPSSystem::FPSUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);

	// Init game
	Game->Init();
}

//------------------------------------------------------------------------------
Engine::~Engine()
{
	Game->Deinit();
	BaseWorld.reset();
	Game.reset();
	RenderingDevice.reset();
	gEngine = nullptr;
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
void Engine::ResizeScreen(const ScreenSize & size)
{
	gConsole.LogDebug("Screen resize: {} {}", size.Width, size.Height);
	GetRenderingDevice()->Resize(size);
}
