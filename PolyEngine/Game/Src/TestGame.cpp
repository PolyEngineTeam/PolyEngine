#include "TestGame.hpp"

#include <Core.hpp>

#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>

void TestGame::Init() 
{
	Camera = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(Camera);
	Engine->GetWorld().AddComponent<Poly::CameraComponent>(Camera, 45.0f, 1.0f, 1000.f);

	TestEnt = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(TestEnt);
	Engine->GetWorld().AddComponent<Poly::MeshRenderingComponent>(TestEnt);

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

void GameMainSystem::GameUpdate(Poly::World * world)
{
	static float val = 0;
	val += 0.001f;
	float y = sinf(val);
	float x = cosf(val);
	
	auto allocator = world->GetComponentAllocator<Poly::CameraComponent>();
	for (Poly::CameraComponent& cameraCmp : *allocator)
	{
		Poly::TransformComponent* transform = cameraCmp.GetSibling<Poly::TransformComponent>();
		transform->SetLocalTranslation(Poly::Vector(x, y * 1.5, 5));
	}
}
