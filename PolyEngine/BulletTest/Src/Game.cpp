#include "Game.hpp"

#include <DeferredTaskSystem.hpp>

#include "GameManager.hpp"
#include "GameManagerWorldComponent.hpp"
#include "CameraMovementSystem.hpp"

DEFINE_GAME(BulletTest)

using namespace Poly;
using namespace BT;


void BulletTest::Init()
{
	gEngine->RegisterGameUpdatePhase(GameManagerSystem::Update);
	gEngine->RegisterGameUpdatePhase(CameraMovementSystem::Update);

	World* world = gEngine->GetWorld();
	DeferredTaskSystem::AddWorldComponentImmediate<BT::GameManagerWorldComponent>(world);

	GameManagerSystem::InitializeDemoWorld(world);
};

void BulletTest::Deinit()
{
}