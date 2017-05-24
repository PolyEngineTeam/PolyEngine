#include "InvadersGame.hpp"

#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <Core.hpp>
#include <HeavyTaskSystem.hpp>

using namespace Poly;

void InvadersGame::Init()
{
	Camera = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(Camera);
	Engine->GetWorld().AddComponent<Poly::CameraComponent>(Camera, 45.0f, 1.0f, 1000.f);
	Engine->GetWorld().AddComponent<Poly::FreeFloatMovementComponent>(Camera, 10.0f, 0.003f);

	// Set some camera position
	Poly::TransformComponent* cameraTrans = Engine->GetWorld().GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(-23.1327f, 13.9473f, -25.7297f));
	cameraTrans->SetLocalRotation(Quaternion(EulerAngles{ 152.154_deg, 52.1159_deg, -180_deg }));

	/*for (int i = -2; i < 2; ++i)
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
	}*/

	Engine->GetWorld().GetViewportWorldComponent().SetCamera(0, Engine->GetWorld().GetComponent<Poly::CameraComponent>(Camera));
	Engine->RegisterUpdatePhase(GameMainSystem::GameUpdate, Poly::Engine::eUpdatePhaseOrder::UPDATE);
};

void InvadersGame::Deinit()
{
	Engine->GetWorld().DestroyEntity(Camera);
	for(auto ent : GameEntities)
		Engine->GetWorld().DestroyEntity(ent);
};

void GameMainSystem::GameUpdate(Poly::World* world)
{
	//test purpuoses, for HeavyTaskSystem
	static int pos = 0;
	static float deg = 0;
	if (pos < 400)
	{
		auto sth = world->SpawnEntity();
		//HeavyTaskSystem::AddComponent<Poly::TransformComponent>(world, sth);
		world->AddComponent<Poly::TransformComponent>(sth);//immediate, we want to edit it now :(
		Poly::TransformComponent* entTransform = world->GetComponent<Poly::TransformComponent>(sth);
		entTransform->SetLocalTranslation(Vector(cos(deg)*10.0f, -10.0f + deg/20.0f, sin(deg)*10.0f));
		deg += 3.0f; pos++;
		HeavyTaskSystem::AddComponent<Poly::CameraComponent>(world, sth, 45.0f, 1.0f, 1000.f);
		HeavyTaskSystem::AddComponent<Poly::FreeFloatMovementComponent>(world, sth, 10.0f, 0.003f);
		HeavyTaskSystem::AddComponent<Poly::MeshRenderingComponent>(world, sth, (const char*)"model-tank/tank.fbx");
		HeavyTaskSystem::RemoveComponent<Poly::FreeFloatMovementComponent>(world, sth);//just testing remove
		HeavyTaskSystem::RemoveComponent<Poly::CameraComponent>(world, sth);
		//HeavyTaskSystem::DestroyEntity(world, sth);
		//HeavyTaskSystem::AddComponent<Poly::FreeFloatMovementComponent>(world, sth, 10.0f, 0.003f);
	}

	/*for (auto components : world->IterateComponents<Poly::MeshRenderingComponent, Poly::TransformComponent>())
	{
		auto transform = std::get<Poly::TransformComponent*>(components);
		if (transform)
		{
			transform->SetLocalRotation(Poly::Quaternion(Poly::Vector::UNIT_Y, Poly::Angle::FromDegrees(val * 300)));
		}
	}*/
}
