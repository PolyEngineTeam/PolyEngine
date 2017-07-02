#include "TestGame.hpp"

#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <Core.hpp>
#include <DeferredTaskSystem.hpp>
#include "ViewportWorldComponent.hpp"

using namespace Poly;

DEFINE_GAME(TestGame)

void TestGame::Init()
{
	Camera = DeferredTaskSystem::SpawnEntityImmediate(Engine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(Engine->GetWorld(), Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(Engine->GetWorld(), Camera, 60_deg, 1.0f, 1000.f);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(Engine->GetWorld(), Camera, 10.0f, 0.003f);

	// Set some camera position
	Poly::TransformComponent* cameraTrans = Engine->GetWorld()->GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(-23.1327f, 13.9473f, -25.7297f));
	cameraTrans->SetLocalRotation(Quaternion(EulerAngles{ 152.154_deg, 52.1159_deg, -180_deg }));

	for (int i = -2; i < 2; ++i)
	{
		for (int j = -2; j < 2; ++j)
		{
			auto ent = DeferredTaskSystem::SpawnEntityImmediate(Engine->GetWorld());
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(Engine->GetWorld(), ent);
			DeferredTaskSystem::AddComponent<Poly::MeshRenderingComponent>(Engine->GetWorld(), ent, (const char*)"model-tank/tank.fbx");
			Poly::TransformComponent* entTransform = Engine->GetWorld()->GetComponent<Poly::TransformComponent>(ent);
			entTransform->SetLocalTranslation(Vector(i * 2, 0, j * 5));
			GameEntities.PushBack(ent);
		}
	}

	Engine->GetWorld()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, Engine->GetWorld()->GetComponent<Poly::CameraComponent>(Camera));
	Engine->RegisterGameUpdatePhase(GameMainSystem::GameUpdate);
};

void TestGame::Deinit()
{
	DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), Camera);
	for (auto ent : GameEntities)
		DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), ent);
};

void GameMainSystem::GameUpdate(Poly::World* /*world*/)
{
	/*for (auto components : world->IterateComponents<Poly::MeshRenderingComponent, Poly::TransformComponent>())
	{
		auto transform = std::get<Poly::TransformComponent*>(components);
		if (transform)
		{
			transform->SetLocalRotation(Poly::Quaternion(Poly::Vector::UNIT_Y, Poly::Angle::FromDegrees(val * 300)));
		}
	}*/
}
