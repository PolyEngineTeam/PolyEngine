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

	Camera = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(world, Camera, 60_deg, 1.0f, 1000.f);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(world, Camera, 10.0f, 0.003f);
	DeferredTaskSystem::AddComponentImmediate<Poly::PostprocessSettingsComponent>(world, Camera);
	Poly::TransformComponent* cameraTrans = world->GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(0.0f, 5.0f, 15.0f));
	// cameraTrans->SetLocalRotation(Quaternion(Vector::UNIT_X, 180_deg));

	world->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, world->GetComponent<Poly::CameraComponent>(Camera));

	Vector DirLightPos = Vector(0.0f, 5.0f, 10.0f);
	Quaternion DirLightRot = Quaternion(Vector::UNIT_Y, -45_deg) * Quaternion(Vector::UNIT_X, -15_deg);
	DirLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, DirLight);
	DeferredTaskSystem::AddComponentImmediate<Poly::DirectionalLightSourceComponent>(world, DirLight, Color(1.0f, 1.0f, 0.9f), 0.5f);
	Poly::TransformComponent* dirLightTrans = world->GetComponent<Poly::TransformComponent>(DirLight);
	dirLightTrans->SetLocalTranslation(DirLightPos);
	dirLightTrans->SetLocalRotation(DirLightRot);

	Vector PointLightPos = Vector(3.0f, -5.0f, 10.0f);
	auto PointLight = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, PointLight);
	DeferredTaskSystem::AddComponentImmediate<Poly::PointLightSourceComponent>(world, PointLight, Color(1.0f, 0.0f, 0.0f), 2000.5f, 1000.0f);
	Poly::TransformComponent* lightTrans = world->GetComponent<Poly::TransformComponent>(PointLight);
	dirLightTrans->SetLocalTranslation(PointLightPos);

	auto Dummy = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, Dummy);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, Dummy, "Models/bullet/bullet.obj", eResourceSource::GAME);
	Poly::TransformComponent* dummyTrans = world->GetComponent<Poly::TransformComponent>(Dummy);
	dummyTrans->SetLocalScale(Vector(0.5f, 0.5f, -1.0f));
	dummyTrans->SetLocalTranslation(DirLightPos);
	dummyTrans->SetLocalRotation(DirLightRot);

	float yPos = (float)Engine->GetRenderingDevice()->GetScreenSize().Height;
	auto textDispaly = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::ScreenSpaceTextComponent>(world, textDispaly, Vector{ 0.0f, yPos ,0.0f }, "Fonts/Raleway/Raleway-Heavy.ttf", eResourceSource::ENGINE, 32, "Kill count: 0");

	auto player = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, player);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, player, "Models/tank2/bradle.3ds", eResourceSource::GAME);
	Poly::TransformComponent* playerTransform = world->GetComponent<Poly::TransformComponent>(player);
	playerTransform->SetLocalTranslation(Vector(-3.0f, 0.0f, 5.0f));
	playerTransform->SetLocalScale(10.0f);
	playerTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -90_deg) * Quaternion(Vector::UNIT_X, -90_deg));

	auto shaderball = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, shaderball);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, shaderball, "Models/shaderball/shaderball.fbx", eResourceSource::GAME);
	Poly::TransformComponent* ballTrans = world->GetComponent<Poly::TransformComponent>(shaderball);
	ballTrans->SetLocalTranslation(Vector(3.0f, 0.0f, 5.0f));
	ballTrans->SetLocalScale(0.01f);
	// entTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -90_deg) * Quaternion(Vector::UNIT_X, -90_deg));

	const float SCALE = 4.0f;
	const float SIZE = 40.0f;

	auto ground = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, ground);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, ground, "Models/ground/ground.fbx", eResourceSource::GAME);
	Poly::TransformComponent* groundTrans = world->GetComponent<Poly::TransformComponent>(ground);
	//	groundTransform->SetLocalTranslation(Vector(SCALE * SIZE, 0.f, SCALE * SIZE));
	groundTrans->SetLocalScale(SCALE);

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

void GameMainSystem::GameUpdate(Poly::World* world)
{
	double time = world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime();
	double deltaTime = TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);

	// int UseCashetes = 0;
	// float ColorTempLuminancePreservation = 0.75f;
	float Distortion	= 0.5f;
	float Bloom			= 0.4f;
	float ColorTempValue = 9500.0f; // 6500.0 from [1000.0, 40000.0]
	float Saturation	= 0.5f;
	float Grain			= 0.5f;
	float Stripes		= 0.5f;
	float Vignette		= 0.5f;
	float Brightness	= 0.1f;
	float Contrast		= 1.5f;

	for (auto cmpTuple : world->IterateComponents<PostprocessSettingsComponent>())
	{
		PostprocessSettingsComponent* postCmp = std::get<PostprocessSettingsComponent*>(cmpTuple);
		float t = SmoothStep(-0.2f, 0.2f, -Cos(Angle::FromRadians(0.5f*time))); // [0, 1]

		postCmp->Distortion = t * Distortion;
	//	postCmp->Bloom = t * Bloom;
		postCmp->ColorTempValue = Lerp(6500.0f, ColorTempValue, t);
		postCmp->Saturation = Lerp(1.0f, Saturation, t);
		postCmp->Grain = Lerp(0.0f, Grain, t);
		postCmp->Stripes = Lerp(0.0f, Stripes, t);
		postCmp->Vignette = Lerp(0.0f, Vignette, t);
		//postCmp->Brightness = Lerp(0.0f, Brightness, t);
		//postCmp->Contrast = Lerp(1.0f, Contrast, t);

		// gConsole.LogInfo("PostCmp: t: {}, Saturation: {}, Vignette: {}", t, postCmp->Saturation, postCmp->Vignette);
	}
}
