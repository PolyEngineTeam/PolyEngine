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
#include <LightSourceComponent.hpp>
#include <PostprocessSettingsComponent.hpp>
#include <ResourceManager.hpp>
#include <SoundSystem.hpp>

#include "GameManagerSystem.hpp"
#include "MovementComponent.hpp"
#include "MovementSystem.hpp"
#include "CollisionComponent.hpp"
#include "CollisionSystem.hpp"
#include "TankComponent.hpp"

using namespace Poly;

DEFINE_GAME(InvadersGame)

void InvadersGame::Init()
{
	Engine->RegisterComponent<PlayerControllerComponent>((int)eGameComponents::PLAYERCONTROLLER);
	Engine->RegisterComponent<BulletComponent>((int)eGameComponents::BULLET);
	Engine->RegisterComponent<GameManagerComponent>((int)eGameComponents::GAMEMANAGER);
	Engine->RegisterComponent<EnemyMovementComponent>((int)eGameComponents::ENEMYMOVEMENT);
	Engine->RegisterComponent<Invaders::MovementSystem::MovementComponent>((int)eGameComponents::MOVEMENT);
	Engine->RegisterComponent<Invaders::CollisionSystem::CollisionComponent>((int)eGameComponents::COLLISION);
	Engine->RegisterComponent<Invaders::TankComponent>((int)eGameComponents::TANK);
	
	World* world = Engine->GetWorld();
	
	Camera = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(world, Camera, 60_deg, 1.0f, 1000.f);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(world, Camera, 10.0f, 0.003f);
	DeferredTaskSystem::AddComponentImmediate<Poly::PostprocessSettingsComponent>(world, Camera);

	float y_pos = (float)Engine->GetRenderingDevice()->GetScreenSize().Height;
	auto textDispaly = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::ScreenSpaceTextComponent>(world, textDispaly, Vector{ 0.0f, y_pos ,0.0f }, "Fonts/Raleway/Raleway-Heavy.ttf", eResourceSource::ENGINE, 32, "Kill count: 0");
	
	GameManager = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<GameManagerComponent>(world, GameManager, textDispaly);
	GameManagerComponent* gameManagerComponent = world->GetComponent<GameManagerComponent>(GameManager);
	
	Light = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, Light);
	DeferredTaskSystem::AddComponentImmediate<Poly::DirectionalLightSourceComponent>(world, Light, 1000.0f);
	Poly::TransformComponent* lightTrans = world->GetComponent<Poly::TransformComponent>(Light);
	lightTrans->SetLocalTranslation(Vector(0.0f, 20.0f, 50.0f));

	// Set some camera position
	Poly::TransformComponent* cameraTrans = world->GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(0.0f, 20.0f, 60.0f));
	cameraTrans->SetLocalRotation(Quaternion({ 1, 0, 0 }, -30_deg));

	for (int i = -10; i < 0; ++i)
	{
		for (int j = -2; j < 1; ++j)
		{
			auto ent = DeferredTaskSystem::SpawnEntityImmediate(world);
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, ent);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, ent, "model-tank/turret.fbx", eResourceSource::GAME);
			Poly::TransformComponent* entTransform = world->GetComponent<Poly::TransformComponent>(ent);

			auto base = DeferredTaskSystem::SpawnEntityImmediate(world);
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, base);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, base, "model-tank/base.fbx", eResourceSource::GAME);
			DeferredTaskSystem::AddComponentImmediate<Invaders::MovementSystem::MovementComponent>(world, base, Vector(5, 0, 0), Vector(0, 0, 0), Quaternion(Vector(0, 0, 0), 0_deg), Quaternion(Vector(0, 0, 0), 0_deg));
			DeferredTaskSystem::AddComponentImmediate<Invaders::CollisionSystem::CollisionComponent>(world, base,  Vector(0, 0, 0), Vector(5.0f, 5.0f, 5.0f));
			DeferredTaskSystem::AddComponentImmediate<Invaders::TankComponent>(world, base,  ent, 12.0_deg, (i * j)%5 );
			Poly::TransformComponent* baseTransform = world->GetComponent<Poly::TransformComponent>(base);
			
			entTransform->SetParent(baseTransform);
			baseTransform->SetLocalTranslation(Vector(i * 12.f, 0.f, j * 8.f));
			baseTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -90.0_deg));
			entTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -60.0_deg));
		}
	}
	auto player = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, player);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, player, "Models/tank2/bradle.3ds", eResourceSource::GAME);
	DeferredTaskSystem::AddComponentImmediate<PlayerControllerComponent>(world, player, 10.0f);
	DeferredTaskSystem::AddComponentImmediate<Poly::SoundEmitterComponent>(world, player, "COJ2_Battle_Hard_Attack.ogg", eResourceSource::GAME);
	Poly::TransformComponent* entTransform = world->GetComponent<Poly::TransformComponent>(player);
	entTransform->SetLocalTranslation(Vector(0, 0, 50));
	entTransform->SetLocalScale(10);
	entTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -90_deg) * Quaternion(Vector::UNIT_X, -90_deg));
	
	world->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, world->GetComponent<Poly::CameraComponent>(Camera));
	Engine->RegisterGameUpdatePhase(Invaders::MovementSystem::MovementUpdatePhase);
	Engine->RegisterGameUpdatePhase(Invaders::CollisionSystem::CollisionUpdatePhase);
	Engine->RegisterGameUpdatePhase(GameMainSystem::GameUpdate);
	Engine->RegisterGameUpdatePhase(ControlSystem::ControlSystemPhase);
	Engine->RegisterGameUpdatePhase(GameManagerSystem::GameManagerSystemPhase);

	for(int x = -1; x <= 1; ++x)
		for (int z = -1; z <= 1; ++z)
		{
			const float SCALE = 4.0f;
			const float SIZE = 40.0f;

			auto ground = DeferredTaskSystem::SpawnEntityImmediate(world);
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, ground);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, ground, "Models/ground/ground.fbx", eResourceSource::GAME);
			Poly::TransformComponent* groundTransform = world->GetComponent<Poly::TransformComponent>(ground);
			groundTransform->SetLocalTranslation(Vector(x * SCALE * SIZE, 0, z * SCALE * SIZE));
			groundTransform->SetLocalScale(SCALE);
		}

	// Precache bullet mesh
	BulletMesh = Poly::ResourceManager<MeshResource>::Load("Models/bullet/lowpolybullet.obj", eResourceSource::GAME);
};

void InvadersGame::Deinit()
{
	DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), Camera);
	for(auto ent : GameEntities)
		DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), ent);
	Poly::ResourceManager<MeshResource>::Release(BulletMesh);
};

void GameMainSystem::GameUpdate(Poly::World* /*world*/)
{
	
}
