#include "Game.hpp"

#include <TimeWorldComponent.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <Core.hpp>
#include <DeferredTaskSystem.hpp>

#include "BackgroundSystem.hpp"
#include "BackgroundComponent.hpp"
#include "ViewportWorldComponent.hpp"

using namespace Poly;

DEFINE_GAME(SGJGame)

enum class eGameComponents
{
	BACKGROUND = (int)Poly::eEngineComponents::_COUNT,
};


void SGJGame::Init()
{
	// Register Game Components
	Engine->RegisterComponent<BackgroundComponent>((int)eGameComponents::BACKGROUND);

	Camera = DeferredTaskSystem::SpawnEntityImmediate(Engine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(Engine->GetWorld(), Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(Engine->GetWorld(), Camera, 60_deg, 1.0f, 1000.f);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(Engine->GetWorld(), Camera, 10.0f, 0.003f);

	// Set some camera position
	Poly::TransformComponent* cameraTrans = Engine->GetWorld()->GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(-23.1327f, 13.9473f, -25.7297f));
	cameraTrans->SetLocalRotation(Quaternion(EulerAngles{ 152.154_deg, 52.1159_deg, -180_deg }));

	// Set background 
	double Time = Engine->GetWorld()->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime();
	Background = DeferredTaskSystem::SpawnEntityImmediate(Engine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::BackgroundComponent>(Engine->GetWorld(), Background, Time);
	// GameManagerComponent* gameManagerComponent = Engine->GetWorld()->GetComponent<GameManagerComponent>(GameManager);
	Engine->RegisterGameUpdatePhase(BackgroundSystem::BackgroundSystemSystemPhase);

	// SETUP SCENE HERE
	Engine->GetWorld()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, Engine->GetWorld()->GetComponent<Poly::CameraComponent>(Camera));
	Engine->RegisterGameUpdatePhase(GameMainSystem::GameUpdate);
};

void SGJGame::Deinit()
{
	DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), Camera);
	for (auto ent : GameEntities)
		DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), ent);
};

void GameMainSystem::GameUpdate(Poly::World* /*world*/)
{
}
