#include "GameManager.hpp"

#include <DeferredTaskSystem.hpp>
#include <ViewportWorldComponent.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <PostprocessSettingsComponent.hpp>
#include <Rigidbody3DComponent.hpp>
#include <Physics3DShapes.hpp>
#include <Physics3DSystem.hpp>
#include <MeshRenderingComponent.hpp>
#include <ConfigBase.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <LightSourceComponent.hpp>
#include <InputSystem.hpp>
#include <InputWorldComponent.hpp>

#include "GameManagerWorldComponent.hpp"

using namespace Poly;

void BT::GameManagerSystem::Update(World* world)
{
	GameManagerWorldComponent* gameManager = Poly::gEngine->GetWorld()->GetWorldComponent<GameManagerWorldComponent>();
	InputWorldComponent* input = world->GetWorldComponent<InputWorldComponent>();

	if (input->IsClicked(eMouseButton::RIGHT))
	{
		TransformComponent* transform = world->GetComponent<TransformComponent>(gameManager->Camera);
		Vector direction = MovementSystem::GetGlobalForward(transform);

		gameManager->Stone = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
		TransformComponent* stoneTransform = DeferredTaskSystem::AddComponentImmediate<TransformComponent>(gEngine->GetWorld(), gameManager->Stone);
		Rigidbody3DComponent* stoneRigidbody = DeferredTaskSystem::AddComponentImmediate<Rigidbody3DComponent>(gEngine->GetWorld(), gameManager->Stone, gEngine->GetWorld(), eRigidBody3DType::DYNAMIC, reinterpret_cast<Physics3DShape*>(&Physics3DSphereShape(1)), 1);
		Physics3DSystem::RegisterRigidbody(world, gameManager->Stone);
		DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, gameManager->Stone, "Models/BulletTest/Sphere.fbx", eResourceSource::GAME);
		world->GetComponent<MeshRenderingComponent>(gameManager->Stone)->SetMaterial(0, PhongMaterial(Color(1, 0.2, 0), Color(1, 0.2, 0), Color(1, 0.2, 0), 8.0f));
		world->GetComponent<MeshRenderingComponent>(gameManager->Stone)->SetShadingModel(eShadingModel::LIT);
		stoneTransform->SetLocalTranslation(transform->GetGlobalTranslation());
		stoneRigidbody->UpdatePosition();
		stoneRigidbody->ApplyGravity();
		stoneRigidbody->ApplyImpulseToCenter(direction * 100);
	}

	for(auto tuple : world->IterateComponents<TransformComponent>())
	{
		TransformComponent* cmp = std::get<TransformComponent*>(tuple);

		if (cmp->GetGlobalTranslation().Length() > 100 && cmp->GetOwnerID() != gameManager->Camera)
			DeferredTaskSystem::DestroyEntity(world, cmp->GetOwnerID());
	}
}

void BT::GameManagerSystem::InitializeDemoWorld(World* world)
{
	GameManagerWorldComponent* gameManager = gEngine->GetWorld()->GetWorldComponent<GameManagerWorldComponent>();

	// create camera
	gameManager->Camera = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(gEngine->GetWorld(), gameManager->Camera);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(gEngine->GetWorld(), gameManager->Camera, 25, 0.01);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(gEngine->GetWorld(), gameManager->Camera, 60_deg, 1.0f, 1000.f);
	gEngine->GetWorld()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, gEngine->GetWorld()->GetComponent<CameraComponent>(gameManager->Camera));
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(gEngine->GetWorld(), gameManager->Camera);
	TransformComponent* cameraTransform = gEngine->GetWorld()->GetComponent<TransformComponent>(gameManager->Camera);
	cameraTransform->SetLocalTranslation(Vector(0, 15, 40));
	cameraTransform->SetLocalRotation(Quaternion({ Angle::FromDegrees(-15), Angle::FromDegrees(0), Angle::FromDegrees(0) }));
	gEngine->GetWorld()->GetComponent<PostprocessSettingsComponent>(gameManager->Camera)->UseFgShader = false;
	gEngine->GetWorld()->GetComponent<PostprocessSettingsComponent>(gameManager->Camera)->UseBgShader = false;

	// create light
	world->GetWorldComponent<AmbientLightWorldComponent>()->SetColor(Color(0.2f, 0.5f, 1.0f));
	world->GetWorldComponent<AmbientLightWorldComponent>()->SetIntensity(0.5f);

	Quaternion DirLightRot = Quaternion(Vector::UNIT_Y, 80_deg) * Quaternion(Vector::UNIT_X, -80_deg);
	Poly::UniqueID KeyDirLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, KeyDirLight);
	DeferredTaskSystem::AddComponentImmediate<Poly::DirectionalLightComponent>(world, KeyDirLight, Color(1.0f, 0.9f, 0.8f), 0.8f);
	Poly::TransformComponent* dirLightTrans = world->GetComponent<Poly::TransformComponent>(KeyDirLight);
	dirLightTrans->SetLocalRotation(DirLightRot);

	// create ground
	gameManager->Ground = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(gEngine->GetWorld(), gameManager->Ground);

	DeferredTaskSystem::AddComponentImmediate<Rigidbody3DComponent>(gEngine->GetWorld(), gameManager->Ground, gEngine->GetWorld(), eRigidBody3DType::STATIC, reinterpret_cast<Physics3DShape*>(&Physics3DBoxShape(Vector(20, 20, 0))));
	Physics3DSystem::RegisterRigidbody(world, gameManager->Ground);

	DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, gameManager->Ground, "Models/BulletTest/Ground.fbx", eResourceSource::GAME);
	world->GetComponent<MeshRenderingComponent>(gameManager->Ground)->SetMaterial(0, PhongMaterial(Color(1, 0.5, 0.2), Color(1, 0.5, 0.2), Color(1, 0.5, 0.2), 8.0f));
	world->GetComponent<MeshRenderingComponent>(gameManager->Ground)->SetShadingModel(eShadingModel::LIT);

	TransformComponent* groundTransform = gEngine->GetWorld()->GetComponent<TransformComponent>(gameManager->Ground);
	Rigidbody3DComponent* groundRigidbody = gEngine->GetWorld()->GetComponent<Rigidbody3DComponent>(gameManager->Ground);
	groundTransform->SetLocalTranslation(Vector(0, 0, 0));
	groundTransform->SetLocalRotation(Quaternion({ -90_deg, 0_deg, 0_deg }));
	groundRigidbody->UpdatePosition();

	// build the wall
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			gameManager->Stone = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
			DeferredTaskSystem::AddComponentImmediate<TransformComponent>(gEngine->GetWorld(), gameManager->Stone);
			DeferredTaskSystem::AddComponentImmediate<Rigidbody3DComponent>(gEngine->GetWorld(), gameManager->Stone, gEngine->GetWorld(), eRigidBody3DType::DYNAMIC, reinterpret_cast<Physics3DShape*>(&Physics3DBoxShape(Vector(1, 1, 1))), 1);
			Physics3DSystem::RegisterRigidbody(world, gameManager->Stone);
			DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, gameManager->Stone, "Models/BulletTest/Cube.fbx", eResourceSource::GAME);
			world->GetComponent<MeshRenderingComponent>(gameManager->Stone)->SetMaterial(0, PhongMaterial(Color(1, 0, 0.2), Color(1, 0, 0.2), Color(1, 0, 0.2), 8.0f));
			world->GetComponent<MeshRenderingComponent>(gameManager->Stone)->SetShadingModel(eShadingModel::LIT);
			TransformComponent* stoneTransform = gEngine->GetWorld()->GetComponent<TransformComponent>(gameManager->Stone);
			Rigidbody3DComponent* stoneRigidbody = gEngine->GetWorld()->GetComponent<Rigidbody3DComponent>(gameManager->Stone);
			stoneTransform->SetLocalTranslation(Vector(-5 + i*2, 5 + j*2, 0));
			stoneRigidbody->UpdatePosition();
			stoneRigidbody->ApplyGravity();
			stoneRigidbody->SetDamping(0.f, 0.f);
		}
	}
}
