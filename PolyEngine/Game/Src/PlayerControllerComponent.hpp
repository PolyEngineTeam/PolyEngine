#pragma once

#include "ComponentBase.hpp"
#include "PlayerUpdateSystem.hpp"
#include "Powerups.hpp"

namespace SGJ
{
	class GAME_DLLEXPORT PlayerControllerComponent : public Poly::ComponentBase
	{
	public:

		PlayerControllerComponent(float movementSpeed = 50.0f, float jumpForce = 250.0f);

		float GetMovementSpeed() const { return MovementSpeed; }
		float GetJumpForce() const { return JumpForce; };
		ePowerup GetActivePowerup() const { return ActivePowerup; };
		Poly::Vector GetMoveVector() const { return CurrentMoveVector; };
		void SetMoveVector(Poly::Vector newMoveVector) { CurrentMoveVector = newMoveVector; };

	private:
		float MovementSpeed;
		float JumpForce;
		ePowerup ActivePowerup = ePowerup::NONE;
		Poly::Vector CurrentMoveVector = Poly::Vector::ZERO;
	};
}
