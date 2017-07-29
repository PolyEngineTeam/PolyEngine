#include "Physics2DColliders.hpp"
#include "PowerupSystem.hpp"
#include "PlayerControllerComponent.hpp"
#include "World.hpp"
#include "Powerups.hpp"
#include "Rigidbody2DComponent.hpp"
#include "TimeSystem.hpp"
#include "TransformComponent.hpp"

using namespace Poly;

namespace SGJ
{
	void PowerupSystem::ApplyInput(World* world, PlayerControllerComponent* playerCmp)
	{
		Poly::RigidBody2DComponent* rbCmp = playerCmp->GetSibling<Poly::RigidBody2DComponent>();
		double deltaTime = TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);
		bool changedSize = false;

		ePowerup activePower = playerCmp->GetActivePowerup();
		Poly::Vector move = playerCmp->GetMoveVector();

		float speedConstraint = 10.0f;

		//check active powerup and modify relevant mechanics
		switch (activePower)
		{
		case(ePowerup::REVERSED_CONTROLS):
			move.X *= -1;
			break;
		case(ePowerup::HIGH_JUMP):
			move.Y *= 2;
			break;
		case(ePowerup::POGO_JUMP):
			//TODO If not jumping, jump
			if (!playerCmp->GetIsJumping())
			{
				move += Vector::UNIT_Y;
				move.Y *= deltaTime * playerCmp->GetJumpForce();
			}
			break;
		case(ePowerup::INCREASED_SPEED):
			move.X *= 2;
			speedConstraint *= 2;
			break;
		case(ePowerup::INCREASED_SIZE):	//not input related, maybe move?
		case(ePowerup::DECREASED_SIZE):
			changedSize = true;
			break;
		default:
			break;
		}

		//check if size changed in this frame
		if ((!changedSize && playerCmp->GetHasChangedSize() ) || (changedSize && !playerCmp->GetHasChangedSize()))
		{
			TransformComponent* transformCmp = playerCmp->GetSibling<TransformComponent>();
			Box2DColliderComponent* colliderCmp = playerCmp->GetSibling<Box2DColliderComponent>();
			Poly::Vector size;
			if (activePower == ePowerup::INCREASED_SIZE)
			{
				playerCmp->SetHasChangedSize(true);
				size = playerCmp->GetDefaultScale() * 2.0f;
			}
			else if(activePower == ePowerup::DECREASED_SIZE)
			{
				playerCmp->SetHasChangedSize(true);
				size = playerCmp->GetDefaultScale() / 2.0f;
			}
			else
			{
				//powerup changed from size-changing to another
				//reset player size to default
				playerCmp->SetHasChangedSize(false);
				//hardcoded for now
				size = playerCmp->GetDefaultScale();
				//colliderCmp->
			}
			transformCmp->SetLocalScale(size);
		}

		//apply validated movement vector
		if (rbCmp->GetLinearSpeed().Length() < speedConstraint)
			rbCmp->ApplyImpulseToCenter(move);

	}
}