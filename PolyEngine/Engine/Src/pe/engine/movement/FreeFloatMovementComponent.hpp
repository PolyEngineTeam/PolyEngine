#pragma once

#include <pe/Defines.hpp>
#include <pe/api/ecs/ComponentBase.hpp>

namespace pe::engine::movement {

	class ENGINE_DLLEXPORT FreeFloatMovementComponent : public ::pe::api::ecs::ComponentBase
	{
		friend class MovementSystem;
	public:
		RTTI_DECLARE_COMPONENT(::pe::engine::movement::FreeFloatMovementComponent) { NO_RTTI_PROPERTY(); }

		FreeFloatMovementComponent(float movementSpeed = 1.0f, float rotationSpeed = 1.0f, float wheelSensitivity = 1.0f);

		float getMovementSpeed() const { return m_movementSpeed; }
		void setMovementSpeed(float value) { m_movementSpeed = value; }
		float getAngularVelocity() const { return m_rotationSpeed; }
		void setAngularVelocity(float value) { m_rotationSpeed = value; }
		float getWheelSensitivity() const { return m_wheelSensitivity; }
		void setWheelSensitivity(float value) { m_wheelSensitivity = value; }

	private:
		float m_movementSpeed = 1.0f;
		float m_rotationSpeed = 1.0f;
		float m_wheelSensitivity = 1.0f;
	};
}

//REGISTER_COMPONENT(::pe::api::ecs::ComponentsIDGroup, ::pe::engine::movement::FreeFloatMovementComponent)