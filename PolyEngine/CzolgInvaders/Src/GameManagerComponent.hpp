#pragma once

#include "ComponentBase.hpp"
#include <Dynarray.hpp>
#include <vector>

namespace Poly {

	class GAME_DLLEXPORT GameManagerComponent : public ComponentBase
	{
		
	public:

		GameManagerComponent(const Poly::UniqueID& counter);
		Poly::Dynarray<Poly::UniqueID>* GetDeadGameEntities() { return &DeadGameEntities; }
		Poly::eKey const GetQuitKey() { return QuitKey; }
		size_t GetKillCount() { return KillCount; }
		Poly::UniqueID& GetKillCounter() { return KillCounter; }

		void SetKillCount(size_t count) { KillCount = count; }

	private:
		Poly::Dynarray<Poly::UniqueID> DeadGameEntities;
		Poly::eKey QuitKey = Poly::eKey::ESCAPE;
		size_t KillCount = 0;
		Poly::UniqueID KillCounter;
	};
}
