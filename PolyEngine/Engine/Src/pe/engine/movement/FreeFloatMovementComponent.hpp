#pragma once

#include <pe/Defines.hpp>
#include <pe/api/ecs/ComponentBase.hpp>
#include <pe/engine/movement/MovementSystem.hpp>

namespace pe::engine::movement {

	class ENGINE_DLLEXPORT FreeFloatMovementComponent : public ::pe::api::ecs::ComponentBase
	{
		friend void MovementSystem::MovementUpdatePhase(api::ecs::Scene*);
	public:
		RTTI_DECLARE_COMPONENT(::pe::engine::movement::FreeFloatMovementComponent) { NO_RTTI_PROPERTY(); }

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
}

//REGISTER_COMPONENT(::pe::api::ecs::ComponentsIDGroup, ::pe::engine::movement::FreeFloatMovementComponent)