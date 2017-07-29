#pragma once

#include <ResourceManager.hpp>
#include <ResourceBase.hpp>
#include <Dynarray.hpp>

namespace SGJ
{
	enum class eTileType
	{
		NOTHING = -1,

		STATICGROUND = 0,
		RIGIDBODYGROUND = 1,

		SPIKESBOTTOM = 32,
		SPIKESTOP = 33,
		SPIKESLEFT = 34,
		SPIKESRIGHT = 35,

		PLAYERSTARTPOS = 2,
		PLAYERENDPOS = 3,

		_COUNT
	};

	class GAME_DLLEXPORT Level : public Poly::BaseObject<>
	{
		friend class LevelComponent;
	public:
		Level(const Poly::String& path);

		Poly::Dynarray<eTileType> Tiles;
		size_t Height;
		size_t Width;
	};
}
