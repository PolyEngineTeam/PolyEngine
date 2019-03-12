#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Movement/MovementSystem.hpp>

namespace Poly {

	class ENGINE_DLLEXPORT FreeFloatMovementComponent : public ComponentBase
	{
		friend void MovementSystem::MovementUpdatePhase(Scene*);
	public:
		RTTI_DECLARE_COMPONENT(::Poly::FreeFloatMovementComponent) { NO_RTTI_PROPERTY(); }

		FreeFloatMovementComponent(float movementSpeed = 1.0f, float rotationSpeed = 1.0f, float wheelSensitivity = 1.0f);

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

	REGISTER_COMPONENT(ComponentsIDGroup, FreeFloatMovementComponent)
}