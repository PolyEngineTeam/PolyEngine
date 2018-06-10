#include "PolyEditorPCH.hpp"

using namespace Poly;

RTTI_DEFINE_TYPE(EditorCameraMovementComponent)

EditorCameraMovementComponent::EditorCameraMovementComponent(float movementSpeed, float rotationSpeed)
	: MovementSpeed(movementSpeed), RotationSpeed(rotationSpeed)
{
}
