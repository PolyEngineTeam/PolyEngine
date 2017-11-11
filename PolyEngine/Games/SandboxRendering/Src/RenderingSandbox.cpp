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
};

void RenderingSandbox::Deinit()
{
	GameManagerSystem::Deinit(gEngine->GetWorld());
};
