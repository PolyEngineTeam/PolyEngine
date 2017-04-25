#include "TestGame.hpp"

#include <Core.hpp>

#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>

void TestGame::Init() 
{
	auto camera = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(camera);
	Engine->GetWorld().AddComponent<Poly::BaseCameraComponent>(camera, 45.0f, 1.0f, 1000.f);

	auto entity = Engine->GetWorld().SpawnEntity();
	Engine->GetWorld().AddComponent<Poly::TransformComponent>(entity);
	Engine->GetWorld().AddComponent<Poly::MeshRenderingComponent>(entity);
};