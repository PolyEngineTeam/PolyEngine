#include "EnginePCH.hpp"

#include "Movement/FreeFloatMovementComponent.hpp"

using namespace Poly;

Poly::FreeFloatMovementComponent::FreeFloatMovementComponent(float movementSpeed, float rotationSpeed)
	: MovementSpeed(movementSpeed), RotationSpeed(rotationSpeed)
{
}
