#pragma once

#include <ResourceManager.hpp>
#include <ResourceBase.hpp>
#include <Dynarray.hpp>

namespace SGJ
{
	enum class eTileType
	{
		NOTHING = -1,
		_COUNT
	};

	class GAME_DLLEXPORT LevelResource : public Poly::BaseObject<>
	{
		friend class LevelComponent;
	public:
		LevelResource(const Poly::String& path);

	private:
		Poly::Dynarray<eTileType> Tiles;
		size_t Height;
		size_t Width;
	};
}
