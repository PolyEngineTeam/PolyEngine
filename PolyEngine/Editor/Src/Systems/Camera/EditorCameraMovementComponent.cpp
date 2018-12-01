#include "PolyEditorPCH.hpp"

using namespace Poly;

RTTI_DEFINE_COMPONENT(EditorCameraMovementComponent)

EditorCameraMovementComponent::EditorCameraMovementComponent(float movementSpeed, float rotationSpeed, float wheelSensitivity)
	: MovementSpeed(movementSpeed), RotationSpeed(rotationSpeed), WheelSensitivity(wheelSensitivity)
{
}

