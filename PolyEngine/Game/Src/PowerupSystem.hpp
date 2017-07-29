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
		void ApplyInput(Poly::World* world, PlayerControllerComponent* player);
	}

}
