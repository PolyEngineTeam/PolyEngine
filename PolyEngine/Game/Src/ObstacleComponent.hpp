#pragma once

#include "ComponentBase.hpp"
#include "Level.hpp"

namespace SGJ
{
	class GAME_DLLEXPORT ObstacleComponent : public Poly::ComponentBase
	{
	public:
		ObstacleComponent(eTileType type);

	private:
		eTileType Type;
	};
}
