#pragma once

#include "ComponentBase.hpp"

namespace Poly {

	class GAME_DLLEXPORT BulletMovementComponent : public ComponentBase
	{

	public:
		BulletMovementComponent(float movementSpeed ,Vector direction );

		float GetMovementSpeed() const { return MovementSpeed; }
		Vector GetDirection() const { return Direction; }
	


	private:
		float MovementSpeed = 1.0f;
		Vector Direction = { 0.0f, 0.0f, 0.0f };
	};
}