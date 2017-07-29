#pragma once

namespace Poly
{
	class World;
}

namespace SGJ
{
	class PlayerControllerComponent;
	namespace PowerupSystem
	{
		void ApplyPowerupsAndInput(Poly::World* world, PlayerControllerComponent* player);
	}

}
