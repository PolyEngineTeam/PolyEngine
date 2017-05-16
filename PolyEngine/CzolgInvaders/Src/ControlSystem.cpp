#include "ControlSystem.hpp"

using namespace Poly;

void ControlSystem::ControlSystemPhase(World* world)
{
	for (auto playerControllerTuple : world->IterateComponents<PlayerControllerComponent>())
	{
		PlayerControllerComponent* player = std::get<PlayerControllerComponent*>(playerControllerTuple);
		Vector move(0,0,0);
		if (world->GetInputWorldComponent().IsPressed(player->GetLeftKey()))
			move -= Vector::UNIT_X;
		else if (world->GetInputWorldComponent().IsPressed(player->GetRightKey()))
			move += Vector::UNIT_X;


		if (move.Length() > 0)
		{
			move.Normalize();
			move *= player->GetMovementSpeed();
			move *= 0.016f;
			TransformComponent* player_transform = player->GetSibling<TransformComponent>();
			Vector prev_location = player_transform->GetLocalTranslation();
			player_transform->SetLocalTranslation(prev_location + move);
		}
		
	}

}