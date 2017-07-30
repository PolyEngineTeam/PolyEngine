#pragma once

#include <ComponentBase.hpp>

#include "Level.hpp"

namespace SGJ
{
	class GAME_DLLEXPORT TileComponent : public Poly::ComponentBase
	{
	public:
		TileComponent(eTileType type) : Type(type) {}

		eTileType GetTileType() const { return Type; }

	private:
		eTileType Type;
	};
}
