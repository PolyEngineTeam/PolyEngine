#include "InvadersGame.hpp"

#include <DeferredTaskSystem.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <ScreenSpaceTextComponent.hpp>
#include <Core.hpp>
#include <DeferredTaskSystem.hpp>
#include <ViewportWorldComponent.hpp>
#include <ResourceManager.hpp>

#include "MovementComponent.hpp"
#include "MovementSystem.hpp"
#include "CollisionComponent.hpp"
#include "CollisionSystem.hpp"

using namespace Poly;

void InvadersGame::Init()
{
	Engine->RegisterComponent<PlayerControllerComponent>((int)eGameComponents::PLAYERCONTROLLER);
	Engine->RegisterComponent<BulletComponent>((int)eGameComponents::BULLET);
	Engine->RegisterComponent<GameManagerComponent>((int)eGameComponents::GAMEMANAGER);
	Engine->RegisterComponent<EnemyMovementComponent>((int)eGameComponents::ENEMYMOVEMENT);
	Engine->RegisterComponent<Invaders::MovementSystem::MovementComponent>((int)eGameComponents::MOVEMENT);
	Engine->RegisterComponent<Invaders::CollisionSystem::CollisionComponent>((int)eGameComponents::COLLISION);
	
	Camera = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(&Engine->GetWorld(), Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(&Engine->GetWorld(), Camera, 45.0f, 1.0f, 1000.f);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(&Engine->GetWorld(), Camera, 10.0f, 0.003f);

	//float x_pos = 100;
	//auto textDispaly = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
	//DeferredTaskSystem::AddComponentImmediate<Poly::ScreenSpaceTextComponent>(&Engine->GetWorld(), textDispaly, Vector{ x_pos, 0.0f ,0.0f }, "Fonts/Raleway/Raleway-Light.ttf", 32, "Kill count: 0");
	
	GameManager = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<GameManagerComponent>(&Engine->GetWorld(), GameManager);
	GameManagerComponent* gameManagerComponent = Engine->GetWorld().GetComponent<GameManagerComponent>(GameManager);
	

	// Set some camera position
	Poly::TransformComponent* cameraTrans = Engine->GetWorld().GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(0.0f, 20.0f, 60.0f));
	cameraTrans->SetLocalRotation(Quaternion({ 1, 0, 0 }, -30_deg));

	for (int i = -6; i < 0; ++i)
	{
		for (int j = -1; j < 1; ++j)
		{
			auto ent = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(&Engine->GetWorld(), ent);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(&Engine->GetWorld(), ent, "model-tank/turret.fbx");
			Poly::TransformComponent* entTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(ent);

			auto base = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(&Engine->GetWorld(), base);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(&Engine->GetWorld(), base, "model-tank/base.fbx");
			DeferredTaskSystem::AddComponentImmediate<Invaders::MovementSystem::MovementComponent>(&Engine->GetWorld(), base, Vector(0, 0, 0), Vector(0, 0, 0), Quaternion(Vector(0, 0, 0), 0_deg), Quaternion(Vector(0, 0, 0), 0_deg));
			DeferredTaskSystem::AddComponentImmediate<Invaders::CollisionSystem::CollisionComponent>(&Engine->GetWorld(), base,  Vector(0, 0, 0), Vector(5.0f, 5.0f, 5.0f));
			Poly::TransformComponent* baseTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(base);
			
			entTransform->SetParent(baseTransform);
			baseTransform->SetLocalTranslation(Vector(i * 12, 0, j * 8));
			baseTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -90.0_deg));
			entTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -90.0_deg));
		}
	}
	auto player = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(&Engine->GetWorld(), player);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(&Engine->GetWorld(), player, "Models/tank2/bradle.3ds");
	DeferredTaskSystem::AddComponentImmediate<PlayerControllerComponent>(&Engine->GetWorld(), player, 10.0f);
	Poly::TransformComponent* entTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(player);
	entTransform->SetLocalTranslation(Vector(0, 0, 50));
	entTransform->SetLocalScale(10);
	entTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -90_deg) * Quaternion(Vector::UNIT_X, -90_deg));
	
	Engine->GetWorld().GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, Engine->GetWorld().GetComponent<Poly::CameraComponent>(Camera));
	Engine->RegisterUpdatePhase(Invaders::MovementSystem::MovementUpdatePhase, Poly::Engine::eUpdatePhaseOrder::UPDATE);
	Engine->RegisterUpdatePhase(Invaders::CollisionSystem::CollisionUpdatePhase, Poly::Engine::eUpdatePhaseOrder::UPDATE);
	Engine->RegisterUpdatePhase(GameMainSystem::GameUpdate, Poly::Engine::eUpdatePhaseOrder::UPDATE);
	Engine->RegisterUpdatePhase(ControlSystem::ControlSystemPhase, Poly::Engine::eUpdatePhaseOrder::UPDATE);

	for(int x = -1; x <= 1; ++x)
		for (int z = -1; z <= 1; ++z)
		{
			const float SCALE = 4.0f;
			const float SIZE = 40.0f;

			auto ground = DeferredTaskSystem::SpawnEntityImmediate(&Engine->GetWorld());
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(&Engine->GetWorld(), ground);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(&Engine->GetWorld(), ground, "Models/ground/ground.fbx");
			Poly::TransformComponent* groundTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(ground);
			groundTransform->SetLocalTranslation(Vector(x * SCALE * SIZE, 0, z * SCALE * SIZE));
			groundTransform->SetLocalScale(SCALE);
		}

	

	// Precache bullet mesh
	BulletMesh = Poly::ResourceManager<GLMeshResource>::Load("Models/bullet/lowpolybullet.obj");
};

void InvadersGame::Deinit()
{
	DeferredTaskSystem::DestroyEntityImmediate(&Engine->GetWorld(), Camera);
	for(auto ent : GameEntities)
		DeferredTaskSystem::DestroyEntityImmediate(&Engine->GetWorld(), ent);
	Poly::ResourceManager<GLMeshResource>::Release(BulletMesh);
};

void GameMainSystem::GameUpdate(Poly::World* /*world*/)
{
	
}
