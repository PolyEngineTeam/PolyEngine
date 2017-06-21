#include "EnginePCH.hpp"


using namespace Poly;

Engine* Poly::gEngine = nullptr;

//------------------------------------------------------------------------------
Engine::Engine(IGame* game, IRenderingDevice* device) : Game(game)
{
	ASSERTE(gEngine == nullptr, "Creating engine twice?");
	gEngine = this;
	BaseWorld = new World(this);
	Game->RegisterEngine(this);
	Renderer = device;
}

//------------------------------------------------------------------------------
Engine::~Engine()
{
	gEngine = nullptr;
	delete BaseWorld;
}

//------------------------------------------------------------------------------
bool Engine::Init()
{
	// Engine Components
	RegisterComponent<TransformComponent>((size_t) eEngineComponents::TRANSFORM);
	RegisterComponent<CameraComponent>((size_t) eEngineComponents::BASE_CAMERA);
	RegisterComponent<MeshRenderingComponent>((size_t) eEngineComponents::MESH_RENDERING);
	RegisterComponent<FreeFloatMovementComponent>((size_t)eEngineComponents::FREE_FLOAT_MOVEMENT);
	RegisterComponent<ScreenSpaceTextComponent>((size_t)eEngineComponents::SCREEN_SPACE_TEXT);
	RegisterComponent<SoundEmitterComponent>((size_t)eEngineComponents::SOUND_EMMITER);
	RegisterComponent<SoundListenerComponent>((size_t)eEngineComponents::SOUND_LISTENER);

	// Engine World Components
	RegisterWorldComponent<InputWorldComponent>((size_t) eEngineWorldComponents::INPUT);
	RegisterWorldComponent<ViewportWorldComponent>((size_t) eEngineWorldComponents::VIEWPORT);
	RegisterWorldComponent<TimeWorldComponent>((size_t) eEngineWorldComponents::TIME);
	RegisterWorldComponent<DebugWorldComponent>((size_t) eEngineWorldComponents::DEBUG);
	RegisterWorldComponent<SoundWorldComponent>((size_t) eEngineWorldComponents::SOUND);

	// Add WorldComponents
	DeferredTaskSystem::AddWorldComponentImmediate<InputWorldComponent>(BaseWorld);
	DeferredTaskSystem::AddWorldComponentImmediate<ViewportWorldComponent>(BaseWorld);
	DeferredTaskSystem::AddWorldComponentImmediate<TimeWorldComponent>(BaseWorld);
	DeferredTaskSystem::AddWorldComponentImmediate<DebugWorldComponent>(BaseWorld);
	DeferredTaskSystem::AddWorldComponentImmediate<SoundWorldComponent>(BaseWorld);

	// Engine update phases
	RegisterUpdatePhase(TimeSystem::TimeUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(InputSystem::InputPhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(MovementSystem::MovementUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(CameraSystem::CameraUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(RenderingSystem::RenderingPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(SoundSystem::SoundPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(DeferredTaskSystem::DeferredTaskPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(FPSSystem::FPSUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);

	SoundSystem::CreateContext(BaseWorld);
	SoundSystem::SetCurrentWorld(BaseWorld);

	Game->Init();

	return true;
}

//------------------------------------------------------------------------------
void Engine::Deinit()
{
	Game->Deinit();
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
void Engine::ResizeScreen(const ScreenSize & size)
{
	gConsole.LogDebug("Screen resize: {} {}", size.Width, size.Height);
	GetRenderingContext()->Resize(size);
}
