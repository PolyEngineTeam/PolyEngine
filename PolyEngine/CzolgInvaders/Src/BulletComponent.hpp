#pragma once

#include "ComponentBase.hpp"
#include <Timer.hpp>
#include <World.hpp>
#include <AARect.hpp>

namespace Poly {

	class GAME_DLLEXPORT BulletComponent : public ComponentBase
	{

	public:
		BulletComponent(float movementSpeed, Vector direction, AARect collision, double spawnTime);

		float GetMovementSpeed() const { return MovementSpeed; }
		Vector GetDirection() const { return Direction; }
		double GetLifeTme() const { return LifeTime; }
		double GetSpawnTme() const { return SpawnTime; }
		AARect& GetCollisionBox() { return CollisionBox; }

	private:
		double LifeTime = 2;
		double SpawnTime = 0.0f;
		float MovementSpeed = 1.0f;
		Vector Direction = { 0.0f, 0.0f, 0.0f };
		AARect CollisionBox;
	};
}