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
		float GetAngularVelocity() const { return RotationSpeed; }

	private:
		float MovementSpeed = 1.0f;
		float RotationSpeed = 1.0f;
	};

	COMPONENT_INS(ComponentsFamily, FreeFloatMovementComponent)
}