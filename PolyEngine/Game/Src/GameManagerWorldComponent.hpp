#pragma once

#include <ComponentBase.hpp>
#include <Dynarray.hpp>
#include <UniqueID.hpp>
#include "Level.hpp"

namespace SGJ
{
	class GAME_DLLEXPORT GameManagerWorldComponent : public Poly::ComponentBase
	{
	public:
		Poly::UniqueID Player;
		Poly::UniqueID Camera;

		Poly::Dynarray<Poly::UniqueID> LevelEntities;
		Poly::Dynarray<Poly::UniqueID> OtherEntities;

		Poly::Dynarray<Level*> Levels;
		size_t CurrentLevelID = 0;
		float MinLevelWidth, MaxLevelWidth;
		float MinLevelHeight, MaxLevelHeight;
	};
}