#include "EnginePCH.hpp"

using namespace Poly;

Engine* Poly::gEngine = nullptr;

//------------------------------------------------------------------------------
Engine::Engine() 
	: Game()
{
	ASSERTE(gEngine == nullptr, "Creating engine twice?");
	gEngine = this;
	RandomSetSeed((int)time(nullptr));
	gAssetsPathConfig.Load();
}

void Engine::Init(std::unique_ptr<IGame> game, std::unique_ptr<IRenderingDevice> device)
{
	if (Editor)
		gAssetsPathConfig.DeserializeFromFile(Editor->GetAssetsPathConfigPath());

	gDebugConfig.Load();
	// also set presets for debug draw (DebugDrawPresets)
	// @todo update debug draw presets from GUI
	gDebugConfig.DebugDrawPresets |= eDebugDrawPreset::GFX;
}

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
	Physics3DConfig physics3DConfig;
	DeferredTaskSystem::AddWorldComponentImmediate<Physics3DWorldComponent>(BaseWorld.get(), physics3DConfig);
	DeferredTaskSystem::AddWorldComponentImmediate<AmbientLightWorldComponent>(BaseWorld.get(), Color(1,1,1,1), 0.2f);
	DeferredTaskSystem::AddWorldComponentImmediate<DebugDrawStateWorldComponent>(BaseWorld.get());

	// Engine update phases
	RegisterUpdatePhase(TimeSystem::TimeUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(InputSystem::InputPhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(Physics2DSystem::Physics2DUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(Physics3DSystem::Physics3DUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	RegisterUpdatePhase(MovementSystem::MovementUpdatePhase, eUpdatePhaseOrder::PREUPDATE);

	RegisterUpdatePhase(PathfindingSystem::UpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(CameraSystem::CameraUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(DebugDrawSystem::DebugRenderingUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(ParticleUpdateSystem::ParticleUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(RenderingSystem::RenderingPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(SoundSystem::SoundPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(DeferredTaskSystem::DeferredTaskPhase, eUpdatePhaseOrder::POSTUPDATE);
	RegisterUpdatePhase(FPSSystem::FPSUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);

	SoundSystem::SetWorldCurrent(BaseWorld.get());

	// Init game
	Game->Init();

	if (Editor)
		Editor->Init();
}

void Engine::Restart()
{
	if (Editor)
		Editor->Deinit();
	Game->Deinit();
	BaseWorld.reset();

	BaseWorld = std::make_unique<World>();

	// Add WorldComponents
	DeferredTaskSystem::AddWorldComponentImmediate<InputWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<ViewportWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<TimeWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<DebugWorldComponent>(BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<SoundWorldComponent>(BaseWorld.get(), BaseWorld.get());
	DeferredTaskSystem::AddWorldComponentImmediate<DeferredTaskWorldComponent>(BaseWorld.get());
	Physics2DConfig physicsConfig;
	DeferredTaskSystem::AddWorldComponentImmediate<Physics2DWorldComponent>(BaseWorld.get(), physicsConfig);
	Physics3DConfig physics3DConfig;
	DeferredTaskSystem::AddWorldComponentImmediate<Physics3DWorldComponent>(BaseWorld.get(), physics3DConfig);
	DeferredTaskSystem::AddWorldComponentImmediate<AmbientLightWorldComponent>(BaseWorld.get(), Color(1, 1, 1, 1), 0.2f);
	DeferredTaskSystem::AddWorldComponentImmediate<DebugDrawStateWorldComponent>(BaseWorld.get());

	SoundSystem::SetWorldCurrent(BaseWorld.get());

	// Init game
	Game->Init();

	if (Editor)
		Editor->Init();
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

void Engine::Update(Dynarray<eUpdatePhaseOrder> phasesToUpdate)
{
	for (auto phase : phasesToUpdate)
		UpdatePhases(phase);
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
