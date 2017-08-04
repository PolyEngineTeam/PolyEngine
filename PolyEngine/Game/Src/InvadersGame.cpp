#include "InvadersGame.hpp"

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

DEFINE_GAME(InvadersGame)

void InvadersGame::Init()
{
	World* world = Engine->GetWorld();
	
	Camera = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, Camera);
	DeferredTaskSystem::AddComponentImmediate<Poly::CameraComponent>(world, Camera, 60_deg, 1.0f, 1000.f);
	DeferredTaskSystem::AddComponentImmediate<Poly::FreeFloatMovementComponent>(world, Camera, 10.0f, 0.003f);
	DeferredTaskSystem::AddComponentImmediate<Poly::PostprocessSettingsComponent>(world, Camera);
	Poly::TransformComponent* cameraTrans = world->GetComponent<Poly::TransformComponent>(Camera);
	cameraTrans->SetLocalTranslation(Vector(0.0f, 20.0f, 60.0f));
	cameraTrans->SetLocalRotation(Quaternion({ 1, 0, 0 }, -30_deg));

	world->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, world->GetComponent<Poly::CameraComponent>(Camera));

	Light = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, Light);
	DeferredTaskSystem::AddComponentImmediate<Poly::DirectionalLightSourceComponent>(world, Light, Color(1.0f, 0.0f, 0.0f), 1000.0f);
	Poly::TransformComponent* lightTrans = world->GetComponent<Poly::TransformComponent>(Light);
	lightTrans->SetLocalTranslation(Vector(0.0f, 20.0f, 50.0f));

	float y_pos = (float)Engine->GetRenderingDevice()->GetScreenSize().Height;
	auto textDispaly = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::ScreenSpaceTextComponent>(world, textDispaly, Vector{ 0.0f, y_pos ,0.0f }, "Fonts/Raleway/Raleway-Heavy.ttf", eResourceSource::ENGINE, 32, "Kill count: 0");
	
	for (int i = -10; i < 0; ++i)
	{
		for (int j = -2; j < 1; ++j)
		{
			auto ent = DeferredTaskSystem::SpawnEntityImmediate(world);
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, ent);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, ent, "model-tank/turret.fbx", eResourceSource::GAME);
			Poly::TransformComponent* entTransform = world->GetComponent<Poly::TransformComponent>(ent);

			auto base = DeferredTaskSystem::SpawnEntityImmediate(world);
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, base);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, base, "model-tank/base.fbx", eResourceSource::GAME);
			Poly::TransformComponent* baseTransform = world->GetComponent<Poly::TransformComponent>(base);
			
			entTransform->SetParent(baseTransform);
			baseTransform->SetLocalTranslation(Vector(i * 12.f, 0.f, j * 8.f));
			baseTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -90.0_deg));
			entTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -60.0_deg));
		}
	}

	auto player = DeferredTaskSystem::SpawnEntityImmediate(world);
	DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, player);
	DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, player, "Models/tank2/bradle.3ds", eResourceSource::GAME);
	Poly::TransformComponent* entTransform = world->GetComponent<Poly::TransformComponent>(player);
	entTransform->SetLocalTranslation(Vector(0, 0, 50));
	entTransform->SetLocalScale(10);
	entTransform->SetLocalRotation(Quaternion(Vector::UNIT_Y, -90_deg) * Quaternion(Vector::UNIT_X, -90_deg));
	
	for(int x = -1; x <= 1; ++x)
	{
		for (int z = -1; z <= 1; ++z)
		{
			const float SCALE = 4.0f;
			const float SIZE = 40.0f;

			auto ground = DeferredTaskSystem::SpawnEntityImmediate(world);
			DeferredTaskSystem::AddComponentImmediate<Poly::TransformComponent>(world, ground);
			DeferredTaskSystem::AddComponentImmediate<Poly::MeshRenderingComponent>(world, ground, "Models/ground/ground.fbx", eResourceSource::GAME);
			Poly::TransformComponent* groundTransform = world->GetComponent<Poly::TransformComponent>(ground);
			groundTransform->SetLocalTranslation(Vector(x * SCALE * SIZE, 0, z * SCALE * SIZE));
			groundTransform->SetLocalScale(SCALE);
		}
	}

	Engine->RegisterGameUpdatePhase(GameMainSystem::GameUpdate);
};

void InvadersGame::Deinit()
{
	DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), Camera);
	for(auto ent : GameEntities)
	{
		DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), ent);
	}
};

void GameMainSystem::GameUpdate(Poly::World* world)
{
	double time = world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime();
	double deltaTime = TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);

	for (auto cmpTuple : world->IterateComponents<PostprocessSettingsComponent>())
	{
		PostprocessSettingsComponent* postCmp = std::get<PostprocessSettingsComponent*>(cmpTuple);
		postCmp->VignetteIntensity = Smoothstep(-0.2f, 0.2f, -Cos(Angle::FromRadians(0.5f*time)));
		gConsole.LogInfo("PostCmp: Vignette: {}", postCmp->VignetteIntensity);
	}
}
