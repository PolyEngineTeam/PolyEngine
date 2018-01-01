#pragma once

#include <ComponentBase.hpp>

namespace BT
{
	class PlayerControllerComponent : public Poly::ComponentBase
	{
	public:
		PlayerControllerComponent() = default;

		float GetVelocity() const { return Velocity; }

	private:
		float Velocity = 0.1;
	};
}