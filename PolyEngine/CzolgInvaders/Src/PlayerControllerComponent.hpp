#pragma once

#include "ComponentBase.hpp"

namespace Poly {

	class GAME_DLLEXPORT PlayerControllerComponent : public ComponentBase
	{
		
	public:
		PlayerControllerComponent(float movementSpeed = 1.0f);

		float GetMovementSpeed() const { return MovementSpeed; }
		Poly::eKey GetLeftKey() const { return left_key; }
		Poly::eKey GetRightKey() const { return right_key; }


	private:
		float MovementSpeed = 1.0f;
		Poly::eKey left_key = Poly::eKey::LEFT;
		Poly::eKey right_key = Poly::eKey::RIGHT;
	};
}