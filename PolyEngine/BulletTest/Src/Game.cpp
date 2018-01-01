#include "Game.hpp"

#include <DeferredTaskSystem.hpp>

#include "GameManager.hpp"
#include "GameManagerWorldComponent.hpp"

DEFINE_GAME(BulletTest)

using namespace Poly;
using namespace BT;


void BulletTest::Init()
{
	gEngine->RegisterGameUpdatePhase(GameManagerSystem::Update);

	World* world = gEngine->GetWorld();
	DeferredTaskSystem::AddWorldComponentImmediate<BT::GameManagerWorldComponent>(world);

	GameManagerSystem::InitializeDemoWorld(world);
};

void BulletTest::Deinit()
{
}