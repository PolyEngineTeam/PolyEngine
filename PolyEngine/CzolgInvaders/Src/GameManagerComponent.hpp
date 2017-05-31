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
		Poly::Dynarray<Poly::UniqueID>* GetSpawnGameEntities() { return &SpawnGameEntities; }
		Poly::eKey const GetQuitKey() { return QuitKey; }
		size_t GetKillCount() { return KillCount; }

		void SetKillCount(size_t count) { KillCount = count; }

	private:
		Poly::Dynarray<Poly::UniqueID> GameEntities;
		Poly::Dynarray<Poly::UniqueID> DeadGameEntities;
		Poly::Dynarray<Poly::UniqueID> SpawnGameEntities;
		Poly::eKey QuitKey = Poly::eKey::ESCAPE;
		size_t KillCount = 0;

	};
}
