#include "PolyEditorPCH.hpp"

using namespace Poly;

RTTI_DEFINE_TYPE(EditorCameraComponent)

EditorCameraComponent::EditorCameraComponent(float movementSpeed, float rotationSpeed)
	: MovementSpeed(movementSpeed), RotationSpeed(rotationSpeed)
{
}
