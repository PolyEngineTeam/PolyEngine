#include "GameManagerSystem.hpp"

#include "GameManagerWorldComponent.hpp"

#include <Core.hpp>
#include <ResourceManager.hpp>
#include <DeferredTaskSystem.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <CameraComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <LightSourceComponent.hpp>
#include <PostprocessSettingsComponent.hpp>
#include <ViewportWorldComponent.hpp>
#include <TimeWorldComponent.hpp>
#include <InputWorldComponent.hpp>

using namespace Poly;

void GameManagerSystem::CreateScene(World* world)
{
	gConsole.LogInfo("GameManagerSystem::CreateScene");

	srand(42);

	GameManagerWorldComponent* GameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();

	UniqueID Camera = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, Camera);
	DeferredTaskSystem::AddComponentImmediate<CameraComponent>(world, Camera, 60_deg, 1.0f, 3000.f);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(world, Camera, 10.0f, 0.003f);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(world, Camera);
	TransformComponent* cameraTrans = world->GetComponent<TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(0.0f, 5.0f, 15.0f));
	GameMgrCmp->Camera;

	world->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, world->GetComponent<CameraComponent>(Camera));
	world->GetWorldComponent<AmbientLightWorldComponent>()->SetColor(Color(0.2f, 0.5f, 1.0f));
	world->GetWorldComponent<AmbientLightWorldComponent>()->SetIntensity(0.1f);

	// Dir Light 0
	Quaternion DirLightRot = Quaternion(Vector::UNIT_Y, -45_deg) * Quaternion(Vector::UNIT_X, -35_deg);
	UniqueID KeyDirLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, KeyDirLight);
	DeferredTaskSystem::AddComponentImmediate<DirectionalLightComponent>(world, KeyDirLight, Color(1.0f, 0.9f, 0.8f), 0.8f);
	TransformComponent* dirLightTrans = world->GetComponent<TransformComponent>(KeyDirLight);
	dirLightTrans->SetLocalRotation(DirLightRot);
	GameMgrCmp->KeyDirLight = KeyDirLight;

	// Point Lights
	CreatePointLight(world, 100.0f);

	AddPointLights(world, 7);

	UniqueID Shaderball = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, Shaderball);
	DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, Shaderball, "Models/shaderball/PolyEngine_shaderball.fbx", eResourceSource::GAME);
	MeshRenderingComponent* ballMesh = world->GetComponent<MeshRenderingComponent>(Shaderball);
	ballMesh->SetMaterial(0, PhongMaterial(Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 0.0f), Color(1.0f, 1.0f, 0.5f), 8.0f));
	ballMesh->SetMaterial(1, PhongMaterial(Color(1.0f, 1.0f, 1.0f), Color(0.4f, 0.4f, 0.4f), Color(1.0f, 1.0f, 0.5f), 16.0f));
	TransformComponent* ballTrans = world->GetComponent<TransformComponent>(Shaderball);
	ballTrans->SetLocalScale(0.1f);
	GameMgrCmp->GameEntities.PushBack(Shaderball);

	UniqueID Ground = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, Ground);
	DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, Ground, "Models/Sponza/sponza.obj", eResourceSource::GAME);
	TransformComponent* groundTrans = world->GetComponent<TransformComponent>(Ground);
	MeshRenderingComponent* sponzaMesh = world->GetComponent<MeshRenderingComponent>(Ground);
	for (int i = 0; i < sponzaMesh->GetMesh()->GetSubMeshes().GetSize(); ++i)
	{
		sponzaMesh->SetMaterial(i, PhongMaterial(Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f), 8.0f));
	}
	GameMgrCmp->GameEntities.PushBack(Ground);

	for (auto cmpTuple : world->IterateComponents<PostprocessSettingsComponent>())
	{
		PostprocessSettingsComponent* postCmp = std::get<PostprocessSettingsComponent*>(cmpTuple);
		postCmp->Distortion = 0.0f;
		postCmp->ColorTempValue = 6500.0f;
		postCmp->Saturation = 1.0f;
		postCmp->Grain = 0.0f;
		postCmp->Stripes = 0.0f;
		postCmp->Vignette = 0.0f;
	}
}

void GameManagerSystem::Update(World* world)
{
	float Time = (float)(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());
	GameManagerWorldComponent* GameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();

	for (int i = 0; i < GameMgrCmp->PointLights.GetSize(); ++i)
	{
		PointLightComponent* PointLightCmp = GameMgrCmp->PointLights[i];
		TransformComponent* TransCmp = PointLightCmp->GetSibling<TransformComponent>();
		Vector Position = GameMgrCmp->PointLightPositions[i];
		Vector Bounce = Vector(0.0f, 100.0f*Abs(Sin(1.0_rad * (Time + 0.1f*(Position.X + Position.Y)))), 0.0f);
		TransCmp->SetLocalTranslation(Position + Bounce);
	}

	InputWorldComponent* inputCmp = world->GetWorldComponent<InputWorldComponent>();
	if (inputCmp->IsReleased(eKey::KEY_G))
	{
		GameMgrCmp->IsDrawingDebugMeshes = !GameMgrCmp->IsDrawingDebugMeshes;

		gConsole.LogInfo("GameManagerSystem::Update IsDrawingDebugMeshes: {}", GameMgrCmp->IsDrawingDebugMeshes);

		for (int i = 0; i < GameMgrCmp->DebugMeshes.GetSize(); ++i)
		{
			GameMgrCmp->DebugMeshes[i]->SetShadingModel(GameMgrCmp->IsDrawingDebugMeshes ? eShadingModel::UNLIT : eShadingModel::NONE);
		}
	}
}

void GameManagerSystem::Deinit(World* world)
{
	gConsole.LogInfo("GameManagerSystem::Cleanup");
}

void GameManagerSystem::AddPointLights(World* world, int Quota)
{
	for (int i = 0; i < Quota; ++i)
	{
		CreatePointLight(world, 200.0f);
	}
}

void GameManagerSystem::CreatePointLight(World* world, float Range)
{
	Vector PointLightPos = Vector(Random(-1.0f, 1.0f), Random(0.0f, 0.2f), Random(-0.5f, 0.5f)) * 1000.0f;
	Color LightColor = Color(1.0f, 0.5f, 0.0f) + Color(Random(0.0f, 1.0f), Random(0.0, 0.5f), Random(0.0f, 0.2f));

	UniqueID PointLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, PointLight);
	DeferredTaskSystem::AddComponentImmediate<PointLightComponent>(world, PointLight, LightColor, 1.0f, Range);
	TransformComponent* PointLightTrans = world->GetComponent<TransformComponent>(PointLight);
	PointLightTrans->SetLocalTranslation(PointLightPos);

	UniqueID PointLightDebugSource = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, PointLightDebugSource);
	DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, PointLightDebugSource, "Models/Primitives/Sphere_LowPoly.obj", eResourceSource::GAME);
	MeshRenderingComponent* PointLightMesh = world->GetComponent<MeshRenderingComponent>(PointLightDebugSource);
	PointLightMesh->SetShadingModel(eShadingModel::UNLIT);
	PointLightMesh->SetMaterial(0, PhongMaterial(LightColor, LightColor, LightColor, 8.0f));
	TransformComponent* PointLightDebugSourceTrans = world->GetComponent<TransformComponent>(PointLightDebugSource);
	PointLightDebugSourceTrans->SetParent(PointLightTrans);
	PointLightDebugSourceTrans->SetLocalScale(10.0f);
	PointLightDebugSourceTrans->SetLocalTranslation(Vector(0.0f, 0.0f, 0.0f));

	UniqueID PointLightDebugRange = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, PointLightDebugRange);
	DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, PointLightDebugRange, "Models/Primitives/Sphere_LowPoly.obj", eResourceSource::GAME);
	MeshRenderingComponent* PointLightRangeMesh = world->GetComponent<MeshRenderingComponent>(PointLightDebugRange);
	PointLightRangeMesh->SetShadingModel(eShadingModel::UNLIT);
	PointLightRangeMesh->SetIsWireframe(true);
	PointLightRangeMesh->SetMaterial(0, PhongMaterial(LightColor, LightColor, LightColor, 8.0f));
	TransformComponent* PointLightRangeTrans = world->GetComponent<TransformComponent>(PointLightDebugRange);
	PointLightRangeTrans->SetParent(PointLightTrans);
	PointLightRangeTrans->SetLocalScale(Range);
	PointLightRangeTrans->SetLocalTranslation(Vector(0.0f, 0.0f, 0.0f));

	PointLightComponent* PointLightCmp = world->GetComponent<PointLightComponent>(PointLight);
	GameManagerWorldComponent* GameMgrCmp = world->GetWorldComponent<GameManagerWorldComponent>();
	GameMgrCmp->PointLights.PushBack(PointLightCmp);
	GameMgrCmp->PointLightPositions.PushBack(PointLightPos);
	GameMgrCmp->DebugMeshes.PushBack(PointLightRangeMesh);
}

float GameManagerSystem::Random(float min, float max)
{
	float rnd = Random();
	return Lerp(min, max, rnd);
}

float GameManagerSystem::Random()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}
