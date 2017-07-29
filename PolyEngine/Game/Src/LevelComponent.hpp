#pragma once

#include <ComponentBase.hpp>

#include "LevelResource.hpp"

namespace SGJ
{
	class GAME_DLLEXPORT LevelComponent : public Poly::ComponentBase
	{
	public:
		LevelComponent(const Poly::String& path);

	private:
		LevelResource* Level;
	};
}
