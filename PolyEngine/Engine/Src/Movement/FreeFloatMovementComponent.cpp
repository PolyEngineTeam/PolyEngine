#include "EnginePCH.hpp"

#include "Movement/FreeFloatMovementComponent.hpp"

using namespace Poly;

Poly::FreeFloatMovementComponent::FreeFloatMovementComponent(float movementSpeed, float rotationSpeed, float wheelSensitivity)
	: MovementSpeed(movementSpeed), RotationSpeed(rotationSpeed), WheelSensitivity(wheelSensitivity)
{
}
