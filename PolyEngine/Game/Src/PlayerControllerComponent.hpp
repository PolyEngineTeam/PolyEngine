#pragma once

#include "ComponentBase.hpp"
#include "PlayerUpdateSystem.hpp"

namespace sgj
{
	class GAME_DLLEXPORT PlayerControllerComponent : public Poly::ComponentBase
	{
		friend void PlayerUpdateSystem::ProcessInput(Poly::World* world);
	public:
		PlayerControllerComponent(float movementSpeed = 1.0f);

		float GetMovementSpeed() const { return MovementSpeed; };


	private:
		//active 'powerup'?
		//deaths?

		//movement speed
		float MovementSpeed;

	};
}
