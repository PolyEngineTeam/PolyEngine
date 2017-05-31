#pragma once

#include "ComponentBase.hpp"
#include <AARect.hpp>

namespace Poly {

	class GAME_DLLEXPORT EnemyMovementComponent : public ComponentBase
	{

	public:
		EnemyMovementComponent(AARect collision, UniqueID& turret, float movementSpeed = 2.0f);

		float GetMovementSpeed() const { return MovementSpeed; }
		AARect& GetCollisionBox() { return CollisionBox; }
		UniqueID GetTurret() { return Turret; }

	private:
		float MovementSpeed = 2.0f;
		AARect CollisionBox;
		UniqueID Turret;
	};
}