#pragma once

#include "ComponentBase.hpp"
#include <Dynarray.hpp>
#include <vector>

namespace Poly {

	class GAME_DLLEXPORT GameManagerComponent : public ComponentBase
	{
		
	public:
		Poly::Dynarray<Poly::UniqueID>* GetGameEntities() { return &GameEntities; }
		Poly::Dynarray<Poly::UniqueID>* GetDeadGameEntities() { return &DeadGameEntities; }

	private:
		Poly::Dynarray<Poly::UniqueID> GameEntities;
		Poly::Dynarray<Poly::UniqueID> DeadGameEntities;

	};
}
