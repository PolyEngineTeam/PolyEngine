#include "EnemyMovementComponent.hpp"

using namespace Poly;

EnemyMovementComponent::EnemyMovementComponent(AARect collision, float movementSpeed) : 
	CollisionBox(collision),
	MovementSpeed(movementSpeed)
{

}