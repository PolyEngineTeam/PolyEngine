#include "TestGame.hpp"

#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <Core.hpp>

using namespace Poly;

void TestGame::Init()
{
	Camera = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(Camera);
	Engine->GetWorld().AddComponent<Poly::CameraComponent>(Camera, 45.0f, 1.0f, 1000.f);

	TestEnt = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(TestEnt);
	Engine->GetWorld().AddComponent<Poly::MeshRenderingComponent>(TestEnt, "Res/model-tank/tank.fbx");
	Poly::TransformComponent* entTransform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(TestEnt);
	entTransform->SetLocalRotation(Poly::Quaternion(Poly::Vector(0, 1, 0), 90_deg));

	Poly::TransformComponent* transform = Engine->GetWorld().GetComponent<Poly::TransformComponent>(Camera);
	transform->SetLocalTranslation(Poly::Vector(0, 0, 5));
	Engine->GetWorld().GetViewportWorldComponent().SetCamera(0, Engine->GetWorld().GetComponent<Poly::CameraComponent>(Camera));

	Engine->RegisterUpdatePhase(GameMainSystem::GameUpdate, Poly::Engine::eUpdatePhaseOrder::UPDATE);
};

void TestGame::Deinit()
{
	Engine->GetWorld().DestroyEntity(Camera);
	Engine->GetWorld().DestroyEntity(TestEnt);
};

void GameMainSystem::GameUpdate(Poly::World* world)
{
	static float val = 0;
	val += 0.001f;
	float y = sinf(val);
	float x = cosf(val);

	for(auto components : world->IterateComponents<Poly::CameraComponent, Poly::TransformComponent>())
	{
		auto transform = std::get<Poly::TransformComponent*>(components);
		if (transform)
		{
			transform->SetLocalTranslation(Poly::Vector(x, y * 3.f, 15));
		}
	}
}
