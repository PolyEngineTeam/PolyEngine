#include "PlayerUpdateSystem.hpp"
#include "World.hpp"
#include "TransformComponent.hpp"
#include "InputWorldComponent.hpp"
#include "PlayerControllerComponent.hpp"

using namespace Poly;

namespace sgj
{
	void PlayerUpdateSystem::ProcessInput(World* world)
	{
		for (auto playerTuple : world->IterateComponents<PlayerControllerComponent, TransformComponent>())
		{
			TransformComponent* transCmp = std::get<TransformComponent*>(playerTuple);
			PlayerControllerComponent* playerCmp = std::get<PlayerControllerComponent*>(playerTuple);


			//movement like in MovementSystem
			Vector move;
			if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_A))
				move -= Vector::UNIT_X;
			else if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_D))
				move += Vector::UNIT_X;

			move *= playerCmp->GetMovementSpeed();
			transCmp->SetLocalTranslation(transCmp->GetLocalTranslation() + transCmp->GetLocalRotation() * move);
		}

	}

}
