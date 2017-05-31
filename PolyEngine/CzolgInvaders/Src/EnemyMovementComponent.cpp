#include "EnemyMovementComponent.hpp"

using namespace Poly;

EnemyMovementComponent::EnemyMovementComponent(AARect collision, UniqueID& turret, float movementSpeed) :
	CollisionBox(collision),
	Turret(turret),
	MovementSpeed(movementSpeed)
{

}