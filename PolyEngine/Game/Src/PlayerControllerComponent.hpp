#pragma once

#include "ComponentBase.hpp"

namespace SGJ
{
	class GAME_DLLEXPORT PlayerControllerComponent : public Poly::ComponentBase
	{
	public:
		PlayerControllerComponent(float movementSpeed = 1.0f);

		float GetMovementSpeed() const { return MovementSpeed; }

	private:
		float MovementSpeed;
	};
}
