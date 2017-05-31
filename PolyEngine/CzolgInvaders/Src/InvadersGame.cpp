#include "InvadersGame.hpp"


using namespace Poly;

void InvadersGame::Init()
{
	Engine->RegisterComponent<PlayerControllerComponent>((int)eGameComponents::PLAYERCONTROLLER);
	Engine->RegisterComponent<BulletComponent>((int)eGameComponents::BULLET);
	Engine->RegisterComponent<GameManagerComponent>((int)eGameComponents::GAMEMANAGER);
	Engine->RegisterComponent<EnemyMovementComponent>((int)eGameComponents::ENEMYMOVEMENT);
	Camera = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(Camera);
	Engine->GetWorld().AddComponent<Poly::CameraComponent>(Camera, 45.0f, 1.0f, 1000.f);
	Engine->GetWorld().AddComponent<Poly::FreeFloatMovementComponent>(Camera, 10.0f, 0.003f);

	
	GameManager = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<GameManagerComponent>(GameManager);
	GameManagerComponent* gameManagerComponent = Engine->GetWorld().GetComponent<GameManagerComponent>(GameManager);

	// Set some camera position
	Poly::TransformComponent* cameraTrans = Engine->GetWorld().GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(0.0f, 20.0f, 60.0f));
	cameraTrans->SetLocalRotation(Quaternion(EulerAngles{ -30_deg, 0_deg, 0_deg }));

	for (int i = -1; i < 1; ++i)
	{
		for (int j = -2; j < 2; ++j)
		{
			auto base = Engine->GetWorld().SpawnEntity();
			Engine->GetWorld().AddComponent<Poly::TransformComponent>(base);
			Engine->GetWorld().AddComponent<Poly::EnemyMovementComponent>(base, AARect(Vector(i * 6, 0, j * 8), Vector(5.0f, 5.0f, 5.0f)));
			Engine->GetWorld().AddComponent<Poly::MeshRenderingComponent>(base, "model-tank/base.fbx");
			Poly::TransformComponent* baseTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(base);
			baseTransform->SetLocalTranslation(Vector(i * 8, 0, j * 10));
			baseTransform->SetLocalRotation(Quaternion(EulerAngles{ 0.0_deg, 180.0_deg, 0.0_deg }));
			gameManagerComponent->GetGameEntities()->PushBack(base);

			auto ent = Engine->GetWorld().SpawnEntity();
			Engine->GetWorld().AddComponent<Poly::TransformComponent>(ent);
			Engine->GetWorld().AddComponent<Poly::MeshRenderingComponent>(ent, "model-tank/turret.fbx");
			Poly::TransformComponent* entTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(ent);
			entTransform->SetLocalTranslation(Vector(i * 8, 0, j * 10));
			entTransform->SetLocalRotation(Quaternion(EulerAngles{ 0.0_deg, 180.0_deg, 0.0_deg }));
			entTransform->SetParent(baseTransform);
			gameManagerComponent->GetGameEntities()->PushBack(ent);
		}
	}
	auto player = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(player);
	Engine->GetWorld().AddComponent<Poly::MeshRenderingComponent>(player, "model-tank/tank.fbx");
	Engine->GetWorld().AddComponent<PlayerControllerComponent>(player, 10.0f);
	Poly::TransformComponent* entTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(player);
	entTransform->SetLocalTranslation(Vector(0, 0, 50));
	gameManagerComponent->GetGameEntities()->PushBack(player);

	Engine->GetWorld().GetViewportWorldComponent().SetCamera(0, Engine->GetWorld().GetComponent<Poly::CameraComponent>(Camera));
	Engine->RegisterUpdatePhase(GameMainSystem::GameUpdate, Poly::Engine::eUpdatePhaseOrder::UPDATE);
	Engine->RegisterUpdatePhase(ControlSystem::ControlSystemPhase, Poly::Engine::eUpdatePhaseOrder::UPDATE);
};

void InvadersGame::Deinit()
{
	Engine->GetWorld().DestroyEntity(Camera);
	for(auto ent : GameEntities)
		Engine->GetWorld().DestroyEntity(ent);
};

void GameMainSystem::GameUpdate(Poly::World* /*world*/)
{
	
}
