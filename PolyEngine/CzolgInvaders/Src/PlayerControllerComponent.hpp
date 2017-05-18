#pragma once

#include "ComponentBase.hpp"

namespace Poly {

	class GAME_DLLEXPORT PlayerControllerComponent : public ComponentBase
	{
		
	public:
		PlayerControllerComponent(float movementSpeed = 1.0f);

		float GetMovementSpeed() const { return MovementSpeed; }
		float GetBulletSpeed() const { return BulletSpeed; }
		Poly::eKey GetLeftKey() const { return LeftKey; }
		Poly::eKey GetRightKey() const { return RightKey; }
		Poly::eKey GetShootKey() const { return ShootKey; }


	private:
		float MovementSpeed = 1.0f;
		float BulletSpeed = 1.0f;
		Poly::eKey LeftKey = Poly::eKey::LEFT;
		Poly::eKey RightKey = Poly::eKey::RIGHT;
		Poly::eKey ShootKey = Poly::eKey::SPACE;
	};
}