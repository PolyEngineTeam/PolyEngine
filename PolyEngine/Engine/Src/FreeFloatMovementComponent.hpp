#pragma once

#include "ComponentBase.hpp"
#include "MovementSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT FreeFloatMovementComponent : public ComponentBase
	{
		friend void MovementSystem::MovementUpdatePhase(World*);
	public:
		FreeFloatMovementComponent(float movementSpeed = 1.0f, float rotationSpeed = 1.0f);

		float GetMovementSpeed() const { return MovementSpeed; }
		void SetMovementSpeed(float value) { MovementSpeed = value; }
		float GetAngularVelocity() const { return RotationSpeed; }

	private:
		float MovementSpeed = 10.0f;
		float RotationSpeed = 10.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, FreeFloatMovementComponent)
}