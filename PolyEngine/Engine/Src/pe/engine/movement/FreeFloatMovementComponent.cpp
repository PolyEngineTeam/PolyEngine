#include <pe/engine/EnginePCH.hpp>

#include <pe/api/ecs/Entity.hpp>
#include <pe/engine/movement/FreeFloatMovementComponent.hpp>

RTTI_DEFINE_COMPONENT(::pe::engine::movement::FreeFloatMovementComponent)

namespace pe::engine::movement {

FreeFloatMovementComponent::FreeFloatMovementComponent(float movementSpeed, float rotationSpeed, float wheelSensitivity)
	: MovementSpeed(movementSpeed), RotationSpeed(rotationSpeed), WheelSensitivity(wheelSensitivity)
{
}

}