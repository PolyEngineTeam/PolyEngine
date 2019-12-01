#include <pe/engine/EnginePCH.hpp>

#include <pe/engine/Engine.hpp>
#include <pe/engine/config/AssetsPathConfig.hpp>
#include <pe/engine/input/InputWorldComponent.hpp>
#include <pe/engine/movement/FreeFloatMovementComponent.hpp>
// #include <Rendering/ViewportWorldComponent.hpp>
// #include <Rendering/Lighting/LightSourceComponent.hpp>
// #include <Rendering/RenderingSystem.hpp>
// #include <Rendering/Particles/ParticleUpdateSystem.hpp>
// #include <Rendering/Animation/AnimationSystem.hpp>
#include <pe/engine/time/TimeWorldComponent.hpp>
//#include <Imgui/ImguiSystem.hpp>

#include <pe/api/app/App.hpp>
#include <pe/api/ecs/Scene.hpp>

namespace pe::engine {

//------------------------------------------------------------------------------
Engine::Engine()
{
	core::math::RandomSetSeed((int)::time(nullptr));
	config::gAssetsPathConfig.Load();
}

//------------------------------------------------------------------------------
/*void Engine::registerApp(api::app::App* app) //init(std::unique_ptr<api::IGame> game, std::unique_ptr<api::rendering::IRenderingDevice> device)
{
	m_app = app;
	m_app->getRenderingDevice()->Init();

	// Engine update phases
	// New seystems, TODO restore them
	//RegisterUpdatePhase(TimeSystem::TimeUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	//RegisterUpdatePhase(InputSystem::InputPhase, eUpdatePhaseOrder::PREUPDATE);
	//RegisterUpdatePhase(MovementSystem::MovementUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	
	// Old systems
	//RegisterSystem(std::make_unique<ImguiSystem>(), eUpdatePhaseOrder::PREUPDATE);
	//RegisterUpdatePhase(Physics2DSystem::Physics2DUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	//RegisterUpdatePhase(Physics3DSystem::Physics3DUpdatePhase, eUpdatePhaseOrder::PREUPDATE);
	//RegisterUpdatePhase(PathfindingSystem::UpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	//RegisterUpdatePhase(CameraSystem::CameraUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	//RegisterUpdatePhase(DebugDrawSystem::DebugRenderingUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	//RegisterUpdatePhase(ParticleUpdateSystem::ParticleUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);
	//RegisterSystem(std::make_unique<AnimationSystem>(), eUpdatePhaseOrder::POSTUPDATE);
	//RegisterUpdatePhase(RenderingSystem::RenderingPhase, eUpdatePhaseOrder::POSTUPDATE);
	//RegisterUpdatePhase(SoundSystem::SoundPhase, eUpdatePhaseOrder::POSTUPDATE);
	//RegisterUpdatePhase(DeferredTaskSystem::DeferredTaskPhase, eUpdatePhaseOrder::POSTUPDATE);
	//RegisterUpdatePhase(FPSSystem::FPSUpdatePhase, eUpdatePhaseOrder::POSTUPDATE);

	loadDefaultScene();

	m_app->getGame()->registerEngine(this);
}*/

//------------------------------------------------------------------------------
void Engine::startGame()
{
	// Placeholder for real scene system
	loadDefaultScene();

	// Init game
	m_app->getGame()->onStart();

	//if (m_app->getEditor())
	//	m_app->getEditor()->OnGameInit();
}

//------------------------------------------------------------------------------
void Engine::endGame()
{
	//if (Editor)
	//	Editor->OnGameDeinit();
	//m_app->getGame()->onEnd();
}

//------------------------------------------------------------------------------
Engine::~Engine()
{
	endGame();
	m_activeScene.reset();
}

//------------------------------------------------------------------------------
void Engine::registerSystem(std::unique_ptr<::pe::api::ecs::ISystem>&& system, api::eEngineUpdatePhase order)
{
	HEAVY_ASSERTE(order != api::eEngineUpdatePhase::_COUNT, "_COUNT enum value passed to RegisterUpdatePhase(), which is an invalid value");
	std::vector<std::unique_ptr<::pe::api::ecs::ISystem>>& UpdatePhases = GameUpdatePhases[order];
	UpdatePhases.push_back(std::move(system));
}


//------------------------------------------------------------------------------
void Engine::update(std::chrono::duration<double> dt)
{
	UpdatePhases(dt, api::eEngineUpdatePhase::PREUPDATE);
	UpdatePhases(dt, api::eEngineUpdatePhase::GAME);
	UpdatePhases(dt, api::eEngineUpdatePhase::POSTUPDATE);
}

void Engine::selectiveUpdate(std::chrono::duration<double> dt, const std::vector<api::eEngineUpdatePhase>& phases)
{
	for (auto phase : phases)
		UpdatePhases(dt, phase);
}

//------------------------------------------------------------------------------
void Engine::ResizeScreen(const core::math::Vector2i& size)
{
	core::utils::gConsole.LogDebug("Screen resize: {} {}", size.X, size.Y);
	GetRenderingDevice()->Resize(size);
}

void Engine::loadDefaultScene()
{
	//@todo(muniu) implement entities staying across scenes.
	//@todo(muniu) implement loading custom scenes.
	m_activeScene = std::make_unique<api::ecs::Scene>();

	// Add WorldComponents
	// DeferredTaskSystem::AddWorldComponentImmediate<InputWorldComponent>(GetActiveScene());
	// DeferredTaskSystem::AddWorldComponentImmediate<ViewportWorldComponent>(GetActiveScene());
	// DeferredTaskSystem::AddWorldComponentImmediate<TimeWorldComponent>(GetActiveScene());
	// DeferredTaskSystem::AddWorldComponentImmediate<DebugWorldComponent>(GetActiveScene());
	// DeferredTaskSystem::AddWorldComponentImmediate<SoundWorldComponent>(GetActiveScene(), GetActiveScene());
	// DeferredTaskSystem::AddWorldComponentImmediate<DeferredTaskWorldComponent>(GetActiveScene());
	// Physics2DConfig physicsConfig;
	// DeferredTaskSystem::AddWorldComponentImmediate<Physics2DWorldComponent>(GetActiveScene(), physicsConfig);
	// Physics3DConfig physics3DConfig;
	// DeferredTaskSystem::AddWorldComponentImmediate<Physics3DWorldComponent>(GetActiveScene(), physics3DConfig);
	// DeferredTaskSystem::AddWorldComponentImmediate<DebugDrawStateWorldComponent>(GetActiveScene());
}

}