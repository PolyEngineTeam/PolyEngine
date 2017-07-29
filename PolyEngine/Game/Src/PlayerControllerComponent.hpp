#pragma once

#include "ComponentBase.hpp"
#include "PlayerUpdateSystem.hpp"
#include "Powerups.hpp"

namespace sgj
{
	class GAME_DLLEXPORT PlayerControllerComponent : public Poly::ComponentBase
	{
		friend void PlayerUpdateSystem::ProcessInput(Poly::World* world);
	public:
		PlayerControllerComponent(float movementSpeed = 1.0f);

		float GetMovementSpeed() const { return MovementSpeed; };
		ePowerup GetActivePower() const { return ActivePower; };
		void SetActivePower(ePowerup activePower) { ActivePower = activePower; };

	private:
		ePowerup ActivePower = ePowerup::NONE;
		bool IsJumping = false;
		int score = 0;
		int deaths = 0;
		float MovementSpeed;

	};
}
