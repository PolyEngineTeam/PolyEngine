#include "PolyEditorPCH.hpp"

#include <Utils/LibraryLoader.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/ViewportWorldComponent.hpp>
#include <Rendering/PostprocessSettingsComponent.hpp>
#include <ECS/DeferredTaskSystem.hpp>

using namespace Poly;

const static float PLAYER_CAMERA_NEAR = 0.1f;
const static float PLAYER_CAMERA_FAR = 300.f;
const static Angle PLAYER_CAMERA_FOV = 60_deg;

EngineManager::EngineManager()
{
	// setup update timer
	Updater = std::make_unique<QTimer>(this);
	connect(Updater.get(), &QTimer::timeout, this, &EngineManager::UpdatePhase);
}

void EngineManager::Init(std::unique_ptr<Poly::IGame> game, std::unique_ptr<Poly::IRenderingDevice> device)
{
	if (Engine)
		throw new EngineManagerException("Creating Engine twice?");

	Engine = std::make_unique<Poly::Engine>();
	Engine->Init(std::move(game), std::move(device));
	Poly::gConsole.LogDebug("Engine created successfully");

	Entity* camera = DeferredTaskSystem::SpawnEntityImmediate(Engine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<CameraComponent>(gEngine->GetWorld(), camera, PLAYER_CAMERA_FOV, PLAYER_CAMERA_NEAR, PLAYER_CAMERA_FAR);
	gEngine->GetWorld()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, gEngine->GetWorld()->GetComponent<CameraComponent>(camera));
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(gEngine->GetWorld(), camera);
	PostprocessSettingsComponent* postCmp = gEngine->GetWorld()->GetComponent<PostprocessSettingsComponent>(camera);
	postCmp->UseBgShader = false;
	postCmp->UseFgShader = true;

	Updater->start(0);
}

void EngineManager::UpdatePhase()
{ 
	Engine->Update();
}