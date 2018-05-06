#include "EnginePCH.hpp"

#include "Movement/FreeFloatMovementComponent.hpp"

using namespace Poly;

RTTI_DEFINE_TYPE(Poly::FreeFloatMovementComponent)

Poly::FreeFloatMovementComponent::FreeFloatMovementComponent(float movementSpeed, float rotationSpeed)
	: MovementSpeed(movementSpeed), RotationSpeed(rotationSpeed)
{
}
