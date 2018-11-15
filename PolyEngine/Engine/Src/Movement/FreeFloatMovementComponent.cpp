#include <EnginePCH.hpp>

#include <Movement/FreeFloatMovementComponent.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::FreeFloatMovementComponent)

Poly::FreeFloatMovementComponent::FreeFloatMovementComponent(float movementSpeed, float rotationSpeed, float wheelSensitivity)
	: MovementSpeed(movementSpeed), RotationSpeed(rotationSpeed), WheelSensitivity(wheelSensitivity)
{
}
