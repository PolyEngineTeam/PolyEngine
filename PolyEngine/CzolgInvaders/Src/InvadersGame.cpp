#include "InvadersGame.hpp"

#include "DeferredTaskSystem.hpp"
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <Core.hpp>
#include <DeferredTaskSystem.hpp>
#include "ViewportWorldComponent.hpp"

using namespace Poly;

void InvadersGame::Init()
{
	Engine->RegisterComponent<PlayerControllerComponent>((int)eGameComponents::PLAYERCONTROLLER);
	Engine->RegisterComponent<BulletComponent>((int)eGameComponents::BULLET);
	Engine->RegisterComponent<GameManagerComponent>((int)eGameComponents::GAMEMANAGER);
	Engine->RegisterComponent<EnemyMovementComponent>((int)eGameComponents::ENEMYMOVEMENT);
	
	Camera = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(&Engine->GetWorld(), Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(&Engine->GetWorld(), Camera, 45.0f, 1.0f, 1000.f);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(&Engine->GetWorld(), Camera, 10.0f, 0.003f);

	
	GameManager = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<GameManagerComponent>(&Engine->GetWorld(), GameManager);
	GameManagerComponent* gameManagerComponent = Engine->GetWorld().GetComponent<GameManagerComponent>(GameManager);

	// Set some camera position
	Poly::TransformComponent* cameraTrans = Engine->GetWorld().GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(0.0f, 20.0f, 60.0f));
	cameraTrans->SetLocalRotation(Quaternion(EulerAngles{ -30_deg, 0_deg, 0_deg }));

	for (int i = -3; i < 3; ++i)
	{
		for (int j = -1; j < 1; ++j)
		{
			auto ent = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(&Engine->GetWorld(), ent);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(&Engine->GetWorld(), ent, "model-tank/turret.fbx");
			Poly::TransformComponent* entTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(ent);
			//entTransform->SetLocalRotation(Quaternion(EulerAngles{ 0.0_deg, -90.0_deg, 0.0_deg }));

			auto base = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(&Engine->GetWorld(), base);
			DeferredTaskSystem::AddComponentImmediate<Poly::EnemyMovementComponent>(&Engine->GetWorld(), base, AABox(Vector(i * 6, 0, j * 8), Vector(5.0f, 5.0f, 5.0f)), ent);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(&Engine->GetWorld(), base, "model-tank/base.fbx");
			Poly::TransformComponent* baseTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(base);
			baseTransform->SetLocalTranslation(Vector(i * 12, 0, j * 8));
			baseTransform->SetLocalRotation(Quaternion(EulerAngles{ 0.0_deg, -90.0_deg, 0.0_deg }));

			entTransform->SetParent(baseTransform);
			
			
		}
	}
	auto player = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(&Engine->GetWorld(), player);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(&Engine->GetWorld(), player, "model-tank/tank.fbx");
	DeferredTaskSystem::AddComponentImmediate<PlayerControllerComponent>(&Engine->GetWorld(), player, 10.0f);
	Poly::TransformComponent* entTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(player);
	entTransform->SetLocalTranslation(Vector(0, 0, 50));
	
	Engine->GetWorld().GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, Engine->GetWorld().GetComponent<Poly::CameraComponent>(Camera));
	Engine->RegisterUpdatePhase(GameMainSystem::GameUpdate, Poly::Engine::eUpdatePhaseOrder::UPDATE);
	Engine->RegisterUpdatePhase(ControlSystem::ControlSystemPhase, Poly::Engine::eUpdatePhaseOrder::UPDATE);
};

void InvadersGame::Deinit()
{
	DeferredTaskSystem::DestroyEntityImmediate(&Engine->GetWorld(), Camera);
	for(auto ent : GameEntities)
		DeferredTaskSystem::DestroyEntityImmediate(&Engine->GetWorld(), ent);
};

void GameMainSystem::GameUpdate(Poly::World* /*world*/)
{
	
}
