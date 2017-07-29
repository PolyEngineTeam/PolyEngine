#include "PlayerUpdateSystem.hpp"
#include "World.hpp"
#include "TransformComponent.hpp"
#include "InputWorldComponent.hpp"
#include "PlayerControllerComponent.hpp"

using namespace Poly;

namespace SGJ
{
	void PlayerUpdateSystem::ProcessInput(World* world)
	{
		//for (auto playerTuple : world->IterateComponents<SGJ::PlayerControllerComponent, TransformComponent>())
		//{
		//	TransformComponent* transCmp = std::get<TransformComponent*>(playerTuple);
		//	SGJ::PlayerControllerComponent* playerCmp = std::get<SGJ::PlayerControllerComponent*>(playerTuple);
		//
		//	//movement like in MovementSystem
		//	Vector move;
		//	if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_A))
		//		move -= Vector::UNIT_X;
		//	else if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_D))
		//		move += Vector::UNIT_X;
		//
		//	move *= playerCmp->GetMovementSpeed();
		//	//Local? (that was in MovementSystem)
		//	transCmp->SetLocalTranslation(transCmp->GetLocalTranslation() + transCmp->GetLocalRotation() * move);
		//}

	}

}
