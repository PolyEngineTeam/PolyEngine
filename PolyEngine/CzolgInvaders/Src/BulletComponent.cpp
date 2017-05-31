#include "BulletComponent.hpp"

using namespace Poly;

BulletComponent::BulletComponent(float movementSpeed, Vector direction, AARect collison, double spawnTime) :
	MovementSpeed(movementSpeed),
	Direction(direction),
	CollisionBox(collison),
	SpawnTime(spawnTime)
{

}