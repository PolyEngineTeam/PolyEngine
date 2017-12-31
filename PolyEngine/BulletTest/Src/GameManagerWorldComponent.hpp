#pragma once

#include "ComponentBase.hpp"

namespace BT
{
	class GAME_DLLEXPORT GameManagerWorldComponent : public Poly::ComponentBase
	{
	public:
		GameManagerWorldComponent() = default;

		Poly::UniqueID Camera;
		Poly::UniqueID Ground;
		Poly::UniqueID Stone;
	};
}