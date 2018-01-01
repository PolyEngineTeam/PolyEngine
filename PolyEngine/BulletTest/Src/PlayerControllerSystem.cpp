#include "PlayerControllerSystem.hpp"

#include <Vector.hpp>
#include <Quaternion.hpp>
#include <TimeSystem.hpp>
#include <World.hpp>
#include <TransformComponent.hpp>

#include "PlayerControllerComponent.hpp"
#include "GameManagerWorldComponent.hpp"

using namespace Poly;

void BT::PlayerControllerSystem::Update(World* world)
{
	float dt = Poly::TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);
	PlayerControllerComponent* player = world->GetComponent<PlayerControllerComponent>(world->GetWorldComponent<GameManagerWorldComponent>()->Player);
	TransformComponent* transform = player->GetSibling<TransformComponent>();


}
