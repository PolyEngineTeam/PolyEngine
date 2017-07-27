#pragma once

#include "ComponentBase.hpp"
#include <AABox.hpp>

namespace Poly {

	class GAME_DLLEXPORT EnemyMovementComponent : public ComponentBase
	{

	public:
		EnemyMovementComponent(const AABox& collision, UniqueID& turret, float movementSpeed = 2.0f);

		float GetMovementSpeed() const { return MovementSpeed; }
		AABox& GetCollisionBox() { return CollisionBox; }
		UniqueID GetTurret() { return Turret; }

	private:
		float MovementSpeed = 2.0f;
		AABox CollisionBox;
		UniqueID Turret;
	};
}