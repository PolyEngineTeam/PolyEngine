#pragma once

#include "ComponentBase.hpp"
#include "PlayerUpdateSystem.hpp"
#include "Powerups.hpp"

namespace SGJ
{
	class GAME_DLLEXPORT PlayerControllerComponent : public Poly::ComponentBase
	{
	public:

		PlayerControllerComponent(float movementSpeed = 3.0f, float jumpForce = 3.0f);

		float GetMovementSpeed() const { return MovementSpeed; }
		float GetJumpForce() const { return JumpForce; }
		bool GetIsJumping() const { return IsJumping; }
		bool GetHasChangedSize() const { return HasChangedSize; }
		ePowerup GetActivePowerup() const { return ActivePowerup; }
		Poly::Vector GetMoveVector() const { return CurrentMoveVector; }
		Poly::Vector GetDefaultScale() const { return DefaultScale; }
		float GetDensityMultiplier() const { return DensityMultiplier; }
		
		void SetIsJumping(bool isJumping) { IsJumping = isJumping; }
		void SetHasChangedSize(bool hasChangedSize) { HasChangedSize = hasChangedSize; }
		void SetMoveVector(Poly::Vector newMoveVector) { CurrentMoveVector = newMoveVector; }
		void SetDensityMultiplier(float densityMul) { DensityMultiplier = densityMul; }
		void SetActivePowerup(ePowerup newPower) { ActivePowerup = newPower; };

		Poly::Vector SpawnPoint;
		double LastAllowJumpChecked = 0;
		bool AllowJump = false;
		double LastLandTimeStart = 0;
		double LastJumpTimeStart = 0;
		double LastDeathTimeStart = -1.0f;
	private:
		float MovementSpeed = 50;
		float JumpForce = 250;
		float DensityMultiplier = 1;
		bool IsJumping = false;
		bool HasChangedSize = false;
		ePowerup ActivePowerup = ePowerup::NONE;
		Poly::Vector CurrentMoveVector = Poly::Vector::ZERO;
		Poly::Vector DefaultScale = Poly::Vector(0.8f, 0.8f, 0.8f);
	};
}
