#include "InvadersGame.hpp"
#include "PlayerControllerComponent.hpp"
#include "ControlSystem.hpp"

#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <Core.hpp>

using namespace Poly;

void InvadersGame::Init()
{
	Engine->RegisterComponent<PlayerControllerComponent>((int)eGameComponents::PLAYERCONTROLLER);
	Camera = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(Camera);
	Engine->GetWorld().AddComponent<Poly::CameraComponent>(Camera, 45.0f, 1.0f, 1000.f);
	Engine->GetWorld().AddComponent<Poly::FreeFloatMovementComponent>(Camera, 10.0f, 0.003f);

	// Set some camera position
	Poly::TransformComponent* cameraTrans = Engine->GetWorld().GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(-23.1327f, 13.9473f, -25.7297f));
	cameraTrans->SetLocalRotation(Quaternion(EulerAngles{ 152.154_deg, 52.1159_deg, -180_deg }));

	for (int i = -2; i < 2; ++i)
	{
		for (int j = -2; j < 2; ++j)
		{
			auto ent = Engine->GetWorld().SpawnEntity();
			Engine->GetWorld().AddComponent<Poly::TransformComponent>(ent);
			Engine->GetWorld().AddComponent<Poly::MeshRenderingComponent>(ent, "model-tank/tank.fbx");
			Poly::TransformComponent* entTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(ent);
			entTransform->SetLocalTranslation(Vector(i * 2, 0, j * 5));
			GameEntities.PushBack(ent);
		}
	}
	auto player = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(player);
	Engine->GetWorld().AddComponent<Poly::MeshRenderingComponent>(player, "model-tank/tank.fbx");
	Engine->GetWorld().AddComponent<PlayerControllerComponent>(player, 10.0f);
	Poly::TransformComponent* entTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(player);
	entTransform->SetLocalTranslation(Vector(0, 0, 30));
	GameEntities.PushBack(player);

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
