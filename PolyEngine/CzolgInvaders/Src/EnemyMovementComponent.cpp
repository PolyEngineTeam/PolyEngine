#include "EnemyMovementComponent.hpp"

using namespace Poly;

EnemyMovementComponent::EnemyMovementComponent(const AABox& collision, UniqueID& turret, float movementSpeed) :
	CollisionBox(collision),
	Turret(turret),
	MovementSpeed(movementSpeed)
{

}