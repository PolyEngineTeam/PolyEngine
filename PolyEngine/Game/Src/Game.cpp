#include "Game.hpp"

#include <TimeWorldComponent.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <Physics2DColliders.hpp>
#include <Rigidbody2DComponent.hpp>
#include <Core.hpp>
#include <DeferredTaskSystem.hpp>

#include "BackgroundSystem.hpp"
#include "BackgroundComponent.hpp"
#include "ViewportWorldComponent.hpp"
#include "GameManagerWorldComponent.hpp"
#include "GameManagerSystem.hpp"
#include "Level.hpp"
#include "GroundComponent.hpp"
#include "ObstacleComponent.hpp"
#include "PlayerControllerComponent.hpp"
#include "PlayerUpdateSystem.hpp"
#include "CameraMovementSystem.hpp"

using namespace Poly;

DEFINE_GAME(SGJGame)

void SGJGame::Init()
{
	// Register Game Components
	gEngine->RegisterComponent<SGJ::GroundComponent>((int)eGameComponents::GROUND);
	gEngine->RegisterComponent<SGJ::ObstacleComponent>((int)eGameComponents::OBSTACLE);
	gEngine->RegisterComponent<SGJ::PlayerControllerComponent>((int)eGameComponents::PLAYER);

	gEngine->RegisterComponent<BackgroundComponent>((int)eGameComponents::BACKGROUND);
	gEngine->RegisterWorldComponent<SGJ::GameManagerWorldComponent>((int)eGameWorldComponents::GAME_MGR);

	gEngine->RegisterGameUpdatePhase(SGJ::PlayerUpdateSystem::Update);
	gEngine->RegisterGameUpdatePhase(SGJ::CameraMovementSystem::CameraMovementUpdatePhase);
	
	
	DeferredTaskSystem::AddWorldComponentImmediate<SGJ::GameManagerWorldComponent>(gEngine->GetWorld());

	SGJ::GameManagerSystem::PrepareNonlevelObjects(gEngine->GetWorld());
		
	UniqueID Camera = gEngine->GetWorld()->GetWorldComponent<SGJ::GameManagerWorldComponent>()->Camera;
	gEngine->GetWorld()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, gEngine->GetWorld()->GetComponent<Poly::CameraComponent>(Camera));
	
	// load levels
	SGJ::GameManagerSystem::LoadLevel(gEngine->GetWorld(), "Levels/Level1.csv");
	SGJ::GameManagerSystem::SpawnLevel(gEngine->GetWorld(), 0);
};

void SGJGame::Deinit()
{
	SGJ::GameManagerSystem::Cleanup(gEngine->GetWorld());
}
