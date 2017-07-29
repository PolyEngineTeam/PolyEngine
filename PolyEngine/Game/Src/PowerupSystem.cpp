#include "PowerupSystem.hpp"
#include "PlayerControllerComponent.hpp"
#include "World.hpp"
#include "Powerups.hpp"
#include "Rigidbody2DComponent.hpp"

namespace SGJ
{
	void PowerupSystem::ApplyInput(PlayerControllerComponent* playerCmp)
	{
		Poly::RigidBody2DComponent* rbCmp = playerCmp->GetSibling<Poly::RigidBody2DComponent>();

		ePowerup activePower = playerCmp->GetActivePowerup();
		Poly::Vector move = playerCmp->GetMoveVector();

		switch (playerCmp->GetActivePowerup())
		{
		case(ePowerup::REVERSED_CONTROLS):
			move.X *= -1;
			break;
		case(ePowerup::HIGH_JUMP):
			move.Y *= 2;
		default:
			break;
		}

		if (rbCmp->GetLinearSpeed().Length() < 10)
			rbCmp->ApplyImpulseToCenter(move);

	}
}