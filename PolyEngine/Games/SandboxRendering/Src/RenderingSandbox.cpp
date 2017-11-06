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

#include "Family.inc"

using namespace Poly;

DEFINE_GAME(RenderingSandbox)

void RenderingSandbox::Init()
{
	srand(42);

	World* world = Engine->GetWorld();

	Camera = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(world, Camera, 60_deg, 1.0f, 1000.f);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(world, Camera, 10.0f, 0.003f);
	DeferredTaskSystem::AddComponentImmediate<Poly::PostprocessSettingsComponent>(world, Camera);
	Poly::TransformComponent* cameraTrans = world->GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(0.0f, 5.0f, 15.0f));
	
	Poly::PostprocessSettingsComponent* postCmp = world->GetComponent<Poly::PostprocessSettingsComponent>(Camera);
	postCmp->Distortion = 0.0f;
	postCmp->ColorTempValue = 6500.0f;
	postCmp->Saturation = 1.0f;
	postCmp->Grain = 0.0f;
	postCmp->Stripes = 0.0f;
	postCmp->Vignette = 0.0f;

	world->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, world->GetComponent<Poly::CameraComponent>(Camera));
	world->GetWorldComponent<AmbientLightWorldComponent>()->SetColor(Color(0.1f, 0.2f, 0.1f));


	Quaternion DirLightRot = Quaternion(Vector::UNIT_Y, -45_deg) * Quaternion(Vector::UNIT_X, -15_deg);
	
	Poly::UniqueID KeyDirLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, KeyDirLight);
	DeferredTaskSystem::AddComponentImmediate<Poly::DirectionalLightComponent>(world, KeyDirLight, Color(1.0f, 0.5f, 0.1f), 0.8f);

	Poly::UniqueID FillDirLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, FillDirLight);
	DeferredTaskSystem::AddComponentImmediate<Poly::DirectionalLightComponent>(world, FillDirLight, Color(0.1f, 0.5f, 1.0f), 0.2f);

	Poly::UniqueID RimDirLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, RimDirLight);
	DeferredTaskSystem::AddComponentImmediate<Poly::DirectionalLightComponent>(world, RimDirLight, Color(0.5f, 0.5f, 1.0f), 1.0f);
	
	
	PointLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, PointLight);
	DeferredTaskSystem::AddComponentImmediate<Poly::PointLightComponent>(world, PointLight, Color(0.0f, 1.0f, 1.0f), 2.0f, 0.1f);
	
	AddPointLights();

	float yPos = (float)Engine->GetRenderingDevice()->GetScreenSize().Height;
	auto textDispaly = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::ScreenSpaceTextComponent>(world, textDispaly, Vector{ 0.0f, yPos ,0.0f }, "Fonts/Raleway/Raleway-Heavy.ttf", eResourceSource::ENGINE, 32, "Kill count: 0");

	auto shaderball = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, shaderball);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, shaderball, "Models/shaderball/shaderball.fbx", eResourceSource::GAME);
	Poly::TransformComponent* ballTrans = world->GetComponent<Poly::TransformComponent>(shaderball);
	ballTrans->SetLocalScale(0.01f);

	const float SCALE = 4.0f;
	const float SIZE = 40.0f;

	auto ground = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, ground);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, ground, "Models/ground/ground.fbx", eResourceSource::GAME);
	Poly::TransformComponent* groundTrans = world->GetComponent<Poly::TransformComponent>(ground);

	Engine->RegisterGameUpdatePhase(GameMainSystem::GameUpdate);
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

void RenderingSandbox::AddPointLights() 
{
	World* world = Engine->GetWorld();

	for (int i = 0; i < 7; ++i)
	{
		Vector Pos = Vector(0.0, 1.0f, 0.0f);
		Vector RndPos = Vector(Random(-1.0f, 1.0f), 0.0f, Random(-1.0f, 1.0f)) * 10.0f;
		Poly::UniqueID PointLight = DeferredTaskSystem::SpawnEntityImmediate(world);
		DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, PointLight);
		DeferredTaskSystem::AddComponentImmediate<Poly::PointLightComponent>(world, PointLight, Color(1.0f, Random(), Random()), 2.0f, 0.1f);
		Poly::TransformComponent* lightTrans = world->GetComponent<Poly::TransformComponent>(PointLight);
		lightTrans->SetLocalTranslation(Pos + RndPos);
	}
}

void GameMainSystem::GameUpdate(Poly::World* world)
{
	float time = (float)(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());
	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY));

	float di = 0;
	for (auto cmpTuple : world->IterateComponents<DirectionalLightComponent, TransformComponent>())
	{
		TransformComponent* dirLightTrans = std::get<TransformComponent*>(cmpTuple);
		Quaternion dirLightRot = Quaternion(Vector::UNIT_X, -45_deg);
		dirLightRot *= Quaternion(Vector::UNIT_Y, 90.0_deg*di + 1_deg *100.0f*time);
		
		dirLightTrans->SetLocalRotation(dirLightRot);

		di += 1.0f;
	}

	int pi = 0;
	for (auto cmpTuple : world->IterateComponents<PointLightComponent, TransformComponent>())
	{
		TransformComponent* pointLightTrans = std::get<TransformComponent*>(cmpTuple);
		PointLightComponent* pointLight = std::get<PointLightComponent*>(cmpTuple);

		// first point light stays in place
		if (pi > 0)
		{
			float x = pointLightTrans->GetLocalTranslation().X;
			pointLight->SetIntensity( 2.0f*Abs(Sin(1.0_rad *(x+time))) );
		
		}
		else
		{
			Vector pointLightPos = Vector(0.0f, 2.0f, 0.0f);
		}
			
		++pi;
	}
}