#pragma once

#include "ComponentBase.hpp"
#include "PlayerUpdateSystem.hpp"
#include "Powerups.hpp"

namespace SGJ
{
	class GAME_DLLEXPORT PlayerControllerComponent : public Poly::ComponentBase
	{
	public:

		PlayerControllerComponent(float movementSpeed = 100.0f, float jumpForce = 100.0f);

		float GetMovementSpeed() const { return MovementSpeed; }
		float GetJumpForce() const { return JumpForce; };
		ePowerup GetActivePowerup() const { return ActivePowerup; };
		Poly::Vector GetMoveVector() const { return CurrentMoveVector; };
		void SetMoveVector(Poly::Vector newMoveVector) { CurrentMoveVector = newMoveVector; };

	private:
		float MovementSpeed = 10.0f;
		float JumpForce = 10.0f;
		ePowerup ActivePowerup = ePowerup::NONE;
		Poly::Vector CurrentMoveVector = Poly::Vector::ZERO;
	};
}
