#pragma once

#include "ComponentBase.hpp"
#include <AARect.hpp>

namespace Poly {

	class GAME_DLLEXPORT EnemyMovementComponent : public ComponentBase
	{

	public:
		EnemyMovementComponent(AARect collision, float movementSpeed = 2.0f);

		float GetMovementSpeed() const { return MovementSpeed; }
		AARect& GetCollisionBox() { return CollisionBox; }

	private:
		float MovementSpeed = 2.0f;
		AARect CollisionBox;
	};
}