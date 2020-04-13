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

#include <pe/engine/time/TimeSystem.hpp>
#include <pe/engine/input/InputSystem.hpp>
#include <pe/engine/movement/MovementSystem.hpp>

#include <pe/api/app/App.hpp>
#include <pe/api/ecs/Scene.hpp>

namespace pe::engine {

//------------------------------------------------------------------------------
Engine::Engine()
{
	core::math::RandomSetSeed((int)::time(nullptr));
	config::gAssetsPathConfig.Load();

	registerSystem(std::make_unique<time::TimeSystem>(), api::eEngineUpdatePhase::PREUPDATE);
	registerSystem(std::make_unique<input::InputSystem>(), api::eEngineUpdatePhase::PREUPDATE);
	registerSystem(std::make_unique<movement::MovementSystem>(), api::eEngineUpdatePhase::PREUPDATE);

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
}

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
	m_app->getGame()->onEnd();
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
	std::vector<std::unique_ptr<::pe::api::ecs::ISystem>>& UpdatePhases = m_updatePhases[order];
	UpdatePhases.push_back(std::move(system));
}


//------------------------------------------------------------------------------
void Engine::update(std::chrono::duration<double> dt)
{
	for (api::eEngineUpdatePhase phase : core::utils::IterateEnum<api::eEngineUpdatePhase>())
	{
		updatePhases(dt, phase);
	}
}

void Engine::selectiveUpdate(std::chrono::duration<double> dt, const std::vector<api::eEngineUpdatePhase>& phases)
{
	for (auto phase : phases)
		updatePhases(dt, phase);
}

//------------------------------------------------------------------------------
void Engine::resizeScreen(const core::math::Vector2i& size)
{
	core::utils::gConsole.LogDebug("Screen resize: {} {}", size.X, size.Y);
	getRenderingDevice()->Resize(size);
}

void Engine::loadDefaultScene()
{
	//@todo(muniu) implement entities staying across scenes.
	//@todo(muniu) implement loading custom scenes.
	m_activeScene = std::make_unique<api::ecs::Scene>();

	m_activeScene->addComponent<input::InputWorldComponent>();
	m_activeScene->addComponent<time::TimeWorldComponent>();

	// Old WorldComponents
	// DeferredTaskSystem::AddWorldComponentImmediate<ViewportWorldComponent>(GetActiveScene());
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