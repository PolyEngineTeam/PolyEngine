#include "EnginePCH.hpp"


using namespace Poly;

Engine* Poly::gEngine = nullptr;

//------------------------------------------------------------------------------
Engine::Engine() 
	: Game()
{
	ASSERTE(gEngine == nullptr, "Creating engine twice?");
	gEngine = this;

	gAssetsPathConfig.Load();
	gDebugConfig.Load();
	// also set presets for debug draw (DebugDrawPresets)
	// TO DO: update debug draw presets from GUI
	gDebugConfig.DebugDrawPresets |= DebugDrawPreset::DYNAMIC;
}

void Poly::Engine::Init(std::unique_ptr<IGame> game, std::unique_ptr<IRenderingDevice> device)
{
	Game = std::move(game);
	RenderingDevice = std::move(device);
	RenderingDevice->Init();
	BaseWorld = std::make_unique<World>();
	Game->RegisterEngine(this);

	// Add WorldComponents
	DeferredTaskSystem::AddWorldComponentImmediate<InputWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<ViewportWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<TimeWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<DebugWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<SoundWorldComponent>(BaseWorld.get(), BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<DeferredTaskWorldComponent>(BaseWorld.get());
	Physics2DConfig physicsConfig;
	DeferredTaskSystem::AddWorldComponentImmediate<Physics2DWorldComponent>(BaseWorld.get(), physicsConfig);
	DeferredTaskSystem::AddWorldComponentImmediate<AmbientLightWorldComponent>(BaseWorld.get(), Color(1,1,1,1), 0.2f);
	DeferredTaskSystem::AddWorldComponentImmediate<DebugRenderingPointsComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<DebugRenderingLinesComponent>(BaseWorld.get());

	// Engine update phases
	RegisterUpdatePhase(TimeSystem::TimeUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(InputSystem::InputPhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(Physics2DSystem::Physics2DUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(MovementSystem::MovementUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(CameraSystem::CameraUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(DebugRenderingSystem::DebugRenderingUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(RenderingSystem::RenderingPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(SoundSystem::SoundPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(DeferredTaskSystem::DeferredTaskPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(FPSSystem::FPSUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);

	SoundSystem::SetWorldCurrent(BaseWorld.get());

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

void Engine::RequestGameQuit()
{
	QuitRequested = true;
}

bool Engine::IsQuitRequested() const { return QuitRequested; }
