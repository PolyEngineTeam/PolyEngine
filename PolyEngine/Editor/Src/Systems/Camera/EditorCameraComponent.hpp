#pragma once

#include "ECS/ComponentBase.hpp"
#include "Systems/Camera/EditorCameraSystem.hpp"

using namespace Poly;

class EditorCameraComponent : public ComponentBase
{
	RTTI_DECLARE_TYPE_DERIVED(EditorCameraComponent, ComponentBase)
	{
		NO_RTTI_PROPERTY()
	}

	friend void EditorCameraSystem::Update(World*);
public:
	EditorCameraComponent(float movementSpeed = 1.0f, float rotationSpeed = 1.0f);

	float GetMovementSpeed() const { return MovementSpeed; }
	void SetMovementSpeed(float value) { MovementSpeed = value; }
	float GetAngularVelocity() const { return RotationSpeed; }

private:
	float MovementSpeed = 1.0f;
	float RotationSpeed = 1.0f;
};