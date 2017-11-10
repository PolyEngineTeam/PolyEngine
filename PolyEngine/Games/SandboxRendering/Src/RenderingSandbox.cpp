#include "stdlib.h" // srand rand

#include "RenderingSandbox.hpp"
#include "GameManagerSystem.hpp"
#include "GameManagerWorldComponent.hpp"

#include <Core.hpp>
#include <DeferredTaskSystem.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <ScreenSpaceTextComponent.hpp>
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
	// Register Game Components
	gEngine->RegisterWorldComponent<GameManagerWorldComponent>((int)eGameWorldComponents::GAME_MGR);
	gEngine->RegisterGameUpdatePhase(GameManagerSystem::Update);
	DeferredTaskSystem::AddWorldComponentImmediate<GameManagerWorldComponent>(gEngine->GetWorld());

	GameManagerSystem::CreateScene(gEngine->GetWorld());


	// UniqueID Camera = gEngine->GetWorld()->GetWorldComponent<GameManagerWorldComponent>()->Camera;
	// gEngine->GetWorld()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, gEngine->GetWorld()->GetComponent<Poly::CameraComponent>(Camera));
	// DeferredTaskSystem::AddComponent<PostprocessSettingsComponent>(gEngine->GetWorld(), Camera);

};

void RenderingSandbox::Deinit()
{
	GameManagerSystem::Deinit(gEngine->GetWorld());

	// DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), Camera);
	// for (auto ent : GameEntities)
	// {
	// 	DeferredTaskSystem::DestroyEntityImmediate(Engine->GetWorld(), ent);
	// }
};

void GameMainSystem::GameUpdate(World* world)
{
	// float time = (float)(world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());
	// float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(world, eEngineTimer::GAMEPLAY));
	// 
	// gConsole.LogInfo("GameMainSystem::GameUpdate: t: {}, dt: {}", time, deltaTime);
}