#include "PolyEditorPCH.hpp"

#include <Utils/LibraryLoader.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/ViewportWorldComponent.hpp>
#include <Rendering/PostprocessSettingsComponent.hpp>
#include <ECS/DeferredTaskSystem.hpp>
#include <Engine.hpp>
#include <Time/TimeWorldComponent.hpp>
#include <Movement/FreeFloatMovementComponent.hpp>

using namespace Poly;

const static float PLAYER_CAMERA_NEAR = 0.1f;
const static float PLAYER_CAMERA_FAR = 300.f;
const static Angle PLAYER_CAMERA_FOV = 60_deg;

EngineManager::EngineManager()
	: Updater(this)
{
	// setup update timer
	connect(&Updater, &QTimer::timeout, this, &EngineManager::UpdatePhase);
}

void EngineManager::InitEngine(std::unique_ptr<IGame> game, std::unique_ptr<IRenderingDevice> device
	, const String& assetsPathConfigPath)
{
	if (Engine)
		throw new EngineManagerException("Creating Engine twice?");

	// create engine instance
	Engine = std::make_unique<Poly::Engine>();
	Engine->RegisterEditor(this);
	gConsole.LogDebug("Engine created successfully");

	// set assets path path form engine
	AssetsPathConfigPath = assetsPathConfigPath;

	EngineState = eEngineState::EDIT;
	// initialize engine
	Engine->Init(std::move(game), std::move(device));
	gConsole.LogDebug("Engine initialized successfully");
}

void EngineManager::Init()
{
	if (EngineState == eEngineState::EDIT)
	{
		// create camera
		// create entity
		Entity* Camera = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
		// add free float movement component
		DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(gEngine->GetWorld(), Camera, 25.f, 0.01f);
		// add camera component
		DeferredTaskSystem::AddComponentImmediate<CameraComponent>(gEngine->GetWorld(), Camera, 60_deg, 1.0f, 1000.f);
		// set camera in world component
		gEngine->GetWorld()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, gEngine->GetWorld()->GetComponent<CameraComponent>(Camera));
		// add postprocess settings component
		DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(gEngine->GetWorld(), Camera);
		gEngine->GetWorld()->GetComponent<PostprocessSettingsComponent>(Camera)->UseFgShader = true;
		gEngine->GetWorld()->GetComponent<PostprocessSettingsComponent>(Camera)->UseBgShader = true;
		
		//Engine->GetWorld()->GetWorldComponent<TimeWorldComponent>()->SetPaused(true);
	}

	gApp->Ui.WorldExplorer->SetObject(Engine->GetWorld());

	// start timer to call update on engine (as fast as possible)
	Updater.start(0);
}

void EngineManager::Deinit()
{
}

void EngineManager::DeinitEngine()
{
	Engine.release();

	EngineState = eEngineState::NONE;
	Updater.stop();
}

void EngineManager::Edit()
{
	switch (EngineState)
	{
	case eEngineState::NONE:
		throw new ProjectManagerException("To switch to edit mode engine must be initialized");

	case eEngineState::EDIT:
		break;

	case eEngineState::GAMEPLAY:
		EngineState = eEngineState::EDIT;
		Engine->Restart();
		break;
	}
}

void EngineManager::Play()
{
	switch (EngineState)
	{
	case eEngineState::NONE:
		throw new ProjectManagerException("To switch to gameplay mode engine must be initialized");

	case eEngineState::EDIT:
		EngineState = eEngineState::GAMEPLAY;
		Engine->Restart();
		break;

	case eEngineState::GAMEPLAY:
		break;
	}
}

void EngineManager::UpdatePhase()
{
	Dynarray<Engine::eUpdatePhaseOrder> updatePhases;

	//updatePhases.PushBack(Engine::eUpdatePhaseOrder::PREUPDATE);
	//updatePhases.PushBack(Engine::eUpdatePhaseOrder::EDITOR);
	//updatePhases.PushBack(Engine::eUpdatePhaseOrder::POSTUPDATE);

	if (EngineState == eEngineState::EDIT)
		Engine->Update(updatePhases);
	else
		Engine->Update();
}