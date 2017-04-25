#include "TestGame.hpp"

#include <Core.hpp>

#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>

void TestGame::Init() 
{
	Camera = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(Camera);
	Engine->GetWorld().AddComponent<Poly::BaseCameraComponent>(Camera, 45.0f, 1.0f, 1000.f);

	TestEnt = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(TestEnt);
	Engine->GetWorld().AddComponent<Poly::MeshRenderingComponent>(TestEnt);
};

void TestGame::Deinit()
{
	Engine->GetWorld().DestroyEntity(Camera);
	Engine->GetWorld().DestroyEntity(TestEnt);
};