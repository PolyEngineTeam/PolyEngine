#include "BulletComponent.hpp"

using namespace Poly;

BulletComponent::BulletComponent(float movementSpeed, Vector direction, AABox collison, double spawnTime) :
	MovementSpeed(movementSpeed),
	Direction(direction),
	CollisionBox(collison),
	SpawnTime(spawnTime)
{

}