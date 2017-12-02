#include "stdlib.h" // srand rand

#include "RenderingSandbox.hpp"

#include <DeferredTaskSystem.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <ScreenSpaceTextComponent.hpp>
#include <Core.hpp>
#include <DeferredTaskSystem.hpp>
#include <ViewportWorldComponent.hpp>
#include <LightSourceComponent.hpp>
#include <PostprocessSettingsComponent.hpp>
#include <ResourceManager.hpp>
#include <TimeWorldComponent.hpp>
#include <SoundSystem.hpp>

using namespace Poly;

DEFINE_GAME(RenderingSandbox)

void RenderingSandbox::Init()
{
	World* world = Engine->GetWorld();

	srand(42);

	Camera = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, Camera);
	DeferredTaskSystem::AddComponentImmediate<CameraComponent>(world, Camera, 60_deg, 1.0f, 3000.f);
	DeferredTaskSystem::AddComponentImmediate<FreeFloatMovementComponent>(world, Camera, 10.0f, 0.003f);
	DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(world, Camera);
	TransformComponent* cameraTrans = world->GetComponent<TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(0.0f, 5.0f, 15.0f));

	world->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, world->GetComponent<CameraComponent>(Camera));
	world->GetWorldComponent<AmbientLightWorldComponent>()->SetColor(Color(0.2f, 0.5f, 1.0f));
	world->GetWorldComponent<AmbientLightWorldComponent>()->SetIntensity(0.1f);
	
	
	// Dir Light 0
	Quaternion DirLightRot = Quaternion(Vector::UNIT_Y, -45_deg) * Quaternion(Vector::UNIT_X, -15_deg);
	UniqueID KeyDirLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, KeyDirLight);
	DeferredTaskSystem::AddComponentImmediate<DirectionalLightComponent>(world, KeyDirLight, Color(1.0f, 0.9f, 0.8f), 0.8f);
	TransformComponent* dirLightTrans = world->GetComponent<TransformComponent>(KeyDirLight);
	dirLightTrans->SetLocalRotation(DirLightRot);

	// SpotLight
	CreateSpotLight(500.0f);

	CreatePointLight(100.0f);
	// AddPointLights(7);
	
	float yPos = (float)Engine->GetRenderingDevice()->GetScreenSize().Height;
	auto textDispaly = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<ScreenSpaceTextComponent>(world, textDispaly, Vector{ 0.0f, yPos ,0.0f }, "Fonts/Raleway/Raleway-Heavy.ttf", eResourceSource::ENGINE, 32, "Kill count: 0");


	auto shaderball = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, shaderball);
	DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, shaderball, "Models/shaderball/PolyEngine_shaderball.fbx", eResourceSource::GAME);
	MeshRenderingComponent* ballMesh = world->GetComponent<MeshRenderingComponent>(shaderball);
	ballMesh->SetMaterial(0, PhongMaterial(Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 0.0f), Color(1.0f, 1.0f, 0.5f), 8.0f));
	ballMesh->SetMaterial(1, PhongMaterial(Color(1.0f, 1.0f, 1.0f), Color(0.4f, 0.4f, 0.4f), Color(1.0f, 1.0f, 0.5f), 16.0f));
	TransformComponent* ballTrans = world->GetComponent<TransformComponent>(shaderball);
	ballTrans->SetLocalScale(0.1f);

	auto ground = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, ground);
	DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, ground, "Models/Sponza/sponza.obj", eResourceSource::GAME);
	TransformComponent* groundTrans = world->GetComponent<TransformComponent>(ground);
	MeshRenderingComponent* sponzaMesh = world->GetComponent<MeshRenderingComponent>(ground);
	for (int i = 0; i < sponzaMesh->GetMesh()->GetSubMeshes().GetSize(); ++i)
	 	sponzaMesh->SetMaterial(i, PhongMaterial(Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f), 8.0f));

	Engine->RegisterGameUpdatePhase(GameMainSystem::GameUpdate);

	for (auto cmpTuple : world->IterateComponents<PostprocessSettingsComponent>())
	{
		PostprocessSettingsComponent* postCmp = std::get<PostprocessSettingsComponent*>(cmpTuple);
		postCmp->Distortion		= 0.0f;
		postCmp->ColorTempValue	= 6500.0f;
		postCmp->Saturation		= 1.0f;
		postCmp->Grain			= 0.0f;
		postCmp->Stripes		= 0.0f;
		postCmp->Vignette		= 0.0f;
	}
};

void RenderingSandbox::Deinit()
{
	DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), Camera);
	for (auto ent : GameEntities)
	{
		DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), ent);
	}
};

float RenderingSandbox::Random()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float RenderingSandbox::Random(float min, float max)
{
	float rnd = Random();
	return Lerp(min, max, rnd);
}

void RenderingSandbox::AddDirectionalLights() 
{
	World* world = Engine->GetWorld();
	Quaternion DirLightRot = Quaternion(Vector::UNIT_Y, -45_deg) * Quaternion(Vector::UNIT_X, -15_deg);

	for (int i = 0; i < 7; ++i)
	{
		UniqueID DirLight = DeferredTaskSystem::SpawnEntityImmediate(world);
		DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, DirLight);
		DeferredTaskSystem::AddComponentImmediate<DirectionalLightComponent>(world, DirLight, Color(Random(), Random(), Random()), 0.4f);
		TransformComponent* dirLightTrans = world->GetComponent<TransformComponent>(DirLight);
		dirLightTrans->SetLocalTranslation(Vector(0.0f, 0.0f, 0.0f));
		dirLightTrans->SetLocalRotation(DirLightRot);
	}
}

void RenderingSandbox::AddPointLights(int Quota) 
{
	for (int i = 0; i < Quota; ++i)
	{
		CreatePointLight(200.0f);
	}
}

void RenderingSandbox::CreatePointLight(float Range)
{
	World* world = Engine->GetWorld();

	Vector PointLightPos = Vector(Random(-1.0f, 1.0f), Random(0.0f, 0.2f), Random(-0.5f, 0.5f)) * 1000.0f;
	Color LightColor = Color(1.0f, 0.5f, 0.0f) + Color(Random(0.0f, 1.0f), Random( 0.0, 0.5f), Random(0.0f, 0.2f));

	// float PointLightRange = 100.0f;
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
}

void RenderingSandbox::CreateSpotLight(float Range)
{
	World* world = Engine->GetWorld();

	Vector SpotLightPos = Vector(100.0f, 100.0f, 0.0f);
	Color LightColor = Color(1.0f, 0.5f, 0.0f) + Color(Random(0.0f, 1.0f), Random(0.0, 0.5f), Random(0.0f, 0.2f));
	Quaternion SpotLightRot = Quaternion(Vector::UNIT_Y, 45_deg) * Quaternion(Vector::UNIT_X, -25_deg);
	// float PointLightRange = 100.0f;
	UniqueID SpotLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, SpotLight);
	DeferredTaskSystem::AddComponentImmediate<SpotLightComponent>(world, SpotLight, LightColor, 1.0f, Range, 5.0f, 17.0f);
	TransformComponent* SpotLightTrans = world->GetComponent<TransformComponent>(SpotLight);
	SpotLightTrans->SetLocalTranslation(SpotLightPos);
	SpotLightTrans->SetLocalRotation(SpotLightRot);

	UniqueID SpotLightDebugSource = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, SpotLightDebugSource);
	DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, SpotLightDebugSource, "Models/Primitives/Sphere_LowPoly.obj", eResourceSource::GAME);
	MeshRenderingComponent* SpotLightMesh = world->GetComponent<MeshRenderingComponent>(SpotLightDebugSource);
	SpotLightMesh->SetShadingModel(eShadingModel::UNLIT);
	SpotLightMesh->SetMaterial(0, PhongMaterial(LightColor, LightColor, LightColor, 8.0f));
	TransformComponent* SpotLightDebugSourceTrans = world->GetComponent<TransformComponent>(SpotLightDebugSource);
	SpotLightDebugSourceTrans->SetParent(SpotLightTrans);
	SpotLightDebugSourceTrans->SetLocalScale(2.0f);
	SpotLightDebugSourceTrans->SetLocalTranslation(Vector(0.0f, 0.0f, 0.0f));

	UniqueID SpotLightDebugRange = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<TransformComponent>(world, SpotLightDebugRange);
	DeferredTaskSystem::AddComponentImmediate<MeshRenderingComponent>(world, SpotLightDebugRange, "Models/Primitives/Sphere_LowPoly.obj", eResourceSource::GAME);
	MeshRenderingComponent* SpotLightRangeMesh = world->GetComponent<MeshRenderingComponent>(SpotLightDebugRange);
	SpotLightRangeMesh->SetShadingModel(eShadingModel::UNLIT);
	SpotLightRangeMesh->SetIsWireframe(true);
	SpotLightRangeMesh->SetMaterial(0, PhongMaterial(LightColor, LightColor, LightColor, 8.0f));
	TransformComponent* SpotLightRangeTrans = world->GetComponent<TransformComponent>(SpotLightDebugRange);
	SpotLightRangeTrans->SetParent(SpotLightTrans);
	SpotLightRangeTrans->SetLocalScale(Vector(0.5f, 1.0f, 0.5f) * Range);
	SpotLightRangeTrans->SetLocalTranslation(Vector(0.0f, 0.0f, 0.0f));
}

void GameMainSystem::GameUpdate(World* world)
{
	float time = (float)(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());
	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(world, eEngineTimer::GAMEPLAY));

	gConsole.LogInfo("GameMainSystem::GameUpdate: t: {}, dt: {}", time, deltaTime);
}