#pragma once

#include "ComponentBase.hpp"

namespace Poly {

	class GAME_DLLEXPORT PlayerControllerComponent : public ComponentBase
	{
		
	public:
		PlayerControllerComponent(float movementSpeed = 1.0f);

		float GetMovementSpeed() const { return MovementSpeed; }
		float GetBulletSpeed() const { return BulletSpeed; }
		double GetShootInterval() const { return ShootInterval; }
		double GetLastShoot() const { return LastShot; }
		float GetMaxAbsXPosition() const { return MaxAbsXPosition; }
		Poly::eKey GetLeftKey() const { return LeftKey; }
		Poly::eKey GetRightKey() const { return RightKey; }
		Poly::eKey GetShootKey() const { return ShootKey; }

		void SetLastShoot(const double time) { LastShot = time; }

	private:
		float MovementSpeed = 2.0f;
		float BulletSpeed = 1.0f;
		double LastShot = 0;
		double ShootInterval = 0.5; // in seconds
		float MaxAbsXPosition = 10.0f;
		Poly::eKey LeftKey = Poly::eKey::LEFT;
		Poly::eKey RightKey = Poly::eKey::RIGHT;
		Poly::eKey ShootKey = Poly::eKey::SPACE;
	};
}