#include "BulletComponent.hpp"

using namespace Poly;

BulletComponent::BulletComponent(float movementSpeed, const Vector& direction, const AABox& collison, double spawnTime) :
	MovementSpeed(movementSpeed),
	Direction(direction),
	CollisionBox(collison),
	SpawnTime(spawnTime)
{

}