#include "PolyEditorPCH.hpp"

using namespace Poly;

EditorCameraMovementComponent::EditorCameraMovementComponent(float movementSpeed, float rotationSpeed, float wheelSensitivity)
	: MovementSpeed(movementSpeed), RotationSpeed(rotationSpeed), WheelSensitivity(wheelSensitivity)
{
}

