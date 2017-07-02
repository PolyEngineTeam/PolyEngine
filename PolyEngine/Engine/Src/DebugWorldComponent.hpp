#pragma once

#include "ComponentBase.hpp"
#include "FPSSystem.hpp"
#include "TimeSystem.hpp"

namespace Poly
{
	class World;

	/// <summary>WorldComponent that holds data for debug system.</summary>
	class ENGINE_DLLEXPORT DebugWorldComponent : public ComponentBase
	{
	friend void FPSSystem::FPSUpdatePhase(World*);
	friend float FPSSystem::GetFPS(World*);

	private:
		FPSSystem::FPSData FPSData;
	};
}