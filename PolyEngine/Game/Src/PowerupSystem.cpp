#include "Physics2DColliders.hpp"
#include "PowerupSystem.hpp"
#include "PlayerControllerComponent.hpp"
#include "PlayerUpdateSystem.hpp"
#include "World.hpp"
#include "Powerups.hpp"
#include "Rigidbody2DComponent.hpp"
#include "TimeSystem.hpp"
#include "TransformComponent.hpp"
#include "Physics2DWorldComponent.hpp"

using namespace Poly;

namespace SGJ
{
	//this function checks which powerup is active and applies its effects
	//TODO it should probably be split into few smaller functions
	void PowerupSystem::ApplyPowerupsAndInput(World* world, PlayerControllerComponent* playerCmp)
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
		case(ePowerup::LOW_JUMP):
			move.Y *= 0.7f;
			break;
		case(ePowerup::POGO_JUMP):
			if (playerCmp->AllowJump)
			{
				//playerCmp->AllowJump = false;
				//move += Vector::UNIT_Y;
				//move.Y *= playerCmp->GetJumpForce();
				PlayerUpdateSystem::PlayerJump(world);
			}
			break;
		case(ePowerup::INCREASED_SPEED):
			move.X *= 2;
			speedConstraint *= 2;
			break;
		case(ePowerup::INCREASED_SIZE):	
		case(ePowerup::DECREASED_SIZE):
			changedSize = true;
			break;
		default:
			break;
		}

		if (activePower == ePowerup::INVERSED_GRAVITY)
		{
			world->GetWorldComponent<Physics2DWorldComponent>()->SetGravity(Vector(0, 9.81f, 0));
			move.Y *= -1;
		}
		else
			world->GetWorldComponent<Physics2DWorldComponent>()->SetGravity(Vector(0, -9.81f, 0));


		//check if size changed in this frame
		if ((!changedSize && playerCmp->GetHasChangedSize() ) || (changedSize && !playerCmp->GetHasChangedSize()))
		{
			TransformComponent* transformCmp = playerCmp->GetSibling<TransformComponent>();
			Circle2DColliderComponent* colliderCmp = playerCmp->GetSibling<Circle2DColliderComponent>();
			 
			Poly::Vector size;
			float colliderRange;
			if (activePower == ePowerup::INCREASED_SIZE)
			{
				playerCmp->SetHasChangedSize(true);
				playerCmp->SetDensityMultiplier(2.0f);		//Setting size density multiplier
				size = playerCmp->GetDefaultScale() * 2.0f;
				colliderRange = playerCmp->GetDefaultScale().X;
			}
			else if(activePower == ePowerup::DECREASED_SIZE)
			{
				playerCmp->SetHasChangedSize(true);
				playerCmp->SetDensityMultiplier(0.5f);		//Setting decr size density multiplier
				size = playerCmp->GetDefaultScale() * 0.5f;
				colliderRange = playerCmp->GetDefaultScale().X * 0.25f;
			}
			else
			{
				//powerup changed from size-changing to another
				//reset player size to default
				playerCmp->SetHasChangedSize(false);
				playerCmp->SetDensityMultiplier(1.0f);
				size = playerCmp->GetDefaultScale();
				colliderRange = playerCmp->GetDefaultScale().X * 0.5f;;
			}
			transformCmp->SetLocalScale(size);
			colliderCmp->SetSize(colliderRange);
			rbCmp->SetDensity(rbCmp->GetDensity() * playerCmp->GetDensityMultiplier());
			
			
		}

		//apply validated movement vector
		if (rbCmp->GetLinearSpeed().Length() < speedConstraint)
			rbCmp->ApplyImpulseToCenter(move * playerCmp->GetDensityMultiplier());

	}
}