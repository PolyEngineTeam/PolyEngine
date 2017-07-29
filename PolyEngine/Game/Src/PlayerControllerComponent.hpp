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
		float GetDensityMultiplier() const { return DensityMultiplier; };
		bool GetIsJumping() const { return IsJumping; };
		bool GetHasChangedSize() const { return HasChangedSize; };
		ePowerup GetActivePowerup() const { return ActivePowerup; };
		Poly::Vector GetMoveVector() const { return CurrentMoveVector; };
		Poly::Vector GetDefaultScale() const { return DefaultScale; };

		void SetIsJumping(bool isJumping) { IsJumping = isJumping; };
		void SetHasChangedSize(bool hasChangedSize) { HasChangedSize = hasChangedSize; };
		void SetMoveVector(const Poly::Vector& newMoveVector) { CurrentMoveVector = newMoveVector; };
		void SetDensityMultiplier(float densityMul) { DensityMultiplier = densityMul; };
		

	private:
		float MovementSpeed = 50.0f;
		float JumpForce = 250.0f;
		float DensityMultiplier = 1.0f;
		bool IsJumping = false;
		bool HasChangedSize = false;
		ePowerup ActivePowerup = ePowerup::NONE;
		Poly::Vector CurrentMoveVector = Poly::Vector::ZERO;
		Poly::Vector DefaultScale = Poly::Vector(0.8f, 0.8f, 0.8f);
	};
}
