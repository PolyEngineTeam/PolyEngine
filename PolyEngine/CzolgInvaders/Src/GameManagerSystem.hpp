#pragma once

#include <World.hpp>
#include "GameManagerComponent.hpp"


namespace Poly
{
	class World;


	namespace GameManagerSystem
	{

		void GameManagerSystemPhase(World*);

		void AddEntity(World*, UniqueID);
		void RemoveEntity(World*, UniqueID);
	}
}

