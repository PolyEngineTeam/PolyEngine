#include "PlayerUpdateSystem.hpp"
#include "World.hpp"
#include "TransformComponent.hpp"
#include "InputWorldComponent.hpp"
#include "PlayerControllerComponent.hpp"
#include "Rigidbody2DComponent.hpp"
#include "Timer.hpp"

using namespace Poly;

namespace SGJ
{
	void PlayerUpdateSystem::Update(World* world)
	{
		double deltaTime = TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);

		for (auto playerTuple : world->IterateComponents<PlayerControllerComponent, RigidBody2DComponent>())
		{
			RigidBody2DComponent* rigidbodyCmp = std::get<RigidBody2DComponent*>(playerTuple);
			PlayerControllerComponent* playerCmp = std::get<PlayerControllerComponent*>(playerTuple);

			Vector move(0, 0, 0);
			if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_A))
				move -= Vector::UNIT_X;
			if (world->GetWorldComponent<InputWorldComponent>()->IsReleased(eKey::KEY_A))
				move += Vector::UNIT_X;
			else if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_D))
				move += Vector::UNIT_X;
			else if (world->GetWorldComponent<InputWorldComponent>()->IsReleased(eKey::KEY_D))
				move -= Vector::UNIT_X;
			else if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::SPACE))
				move += Vector::UNIT_Y;

			move *= deltaTime * playerCmp->GetMovementSpeed();

			if (rigidbodyCmp->GetLinearSpeed().Length() < 1)
				rigidbodyCmp->ApplyImpulseToCenter(move);

			//rigidbodyCmp->
		
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
		}

	}

}
