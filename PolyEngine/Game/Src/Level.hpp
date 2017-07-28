#pragma once

#include "ResourceManager.hpp"
#include "Dynarray.hpp"

namespace SGJ
{
	enum class eTileType
	{
		NONE = -1,
		GROUND = 0,
		_COUNT
	};

	class Level : Poly::ResourceBase
	{
	public:
		Level(const Poly::String& path);


	private:
		Poly::Dynarray<eTileType> Tiles;
		size_t Height, Width;
	};
}