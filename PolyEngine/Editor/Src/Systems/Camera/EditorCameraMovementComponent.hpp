#pragma once

#include "ECS/ComponentBase.hpp"
#include "Systems/Camera/EditorCameraMovementSystem.hpp"

using namespace Poly;

class EditorCameraMovementComponent : public ComponentBase
{
	RTTI_DECLARE_COMPONENT(EditorCameraMovementComponent) { NO_RTTI_PROPERTY(); }

	friend void EditorCameraMovementSystem::Update(Scene*);
public:
	EditorCameraMovementComponent(float movementSpeed = 1.0f, float rotationSpeed = 1.0f, float wheelSensitivity = 1.0f);

	float GetMovementSpeed() const { return MovementSpeed; }
	void SetMovementSpeed(float value) { MovementSpeed = value; }
	float GetAngularVelocity() const { return RotationSpeed; }
	void SetAngularVelocity(float value) { RotationSpeed = value; }
	float GetWheelSensitivity() const { return WheelSensitivity; }
	void SetWheelSensitivity(float value) { WheelSensitivity = value; }

private:
	float MovementSpeed = 1.0f;
	float RotationSpeed = 1.0f;
	float WheelSensitivity = 1.0f;
};

REGISTER_COMPONENT(ComponentsIDGroup, EditorCameraMovementComponent)