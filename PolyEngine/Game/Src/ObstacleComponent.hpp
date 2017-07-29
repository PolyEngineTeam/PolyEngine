#pragma once

#include "ComponentBase.hpp"
#include "Level.hpp"

namespace SGJ
{
	class GAME_DLLEXPORT ObstacleComponent : public Poly::ComponentBase
	{
	public:
		ObstacleComponent(eTileType type);

		eTileType GetTileType() const{ return Type; }

	private:
		eTileType Type;
	};
}
