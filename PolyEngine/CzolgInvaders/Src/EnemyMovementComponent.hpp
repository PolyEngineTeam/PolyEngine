#pragma once

#include "ComponentBase.hpp"

namespace Poly {

	class GAME_DLLEXPORT EnemyMovementComponent : public ComponentBase
	{

	public:
		EnemyMovementComponent(float movementSpeed = 2.0f);

		float GetMovementSpeed() const { return MovementSpeed; }

	private:
		float MovementSpeed = 2.0f;
	};
}