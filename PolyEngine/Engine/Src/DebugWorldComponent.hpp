#pragma once

#include "ComponentBase.hpp"
#include "FPSSystem.hpp"
#include "TimeSystem.hpp"

namespace Poly
{
	class World;

	namespace FPSSystem
	{
		struct FPSData
		{
			int FPS;
			double ElapsedTime;
			bool DisplayingFPS;
		};
	}

	class ENGINE_DLLEXPORT DebugWorldComponent : public ComponentBase
	{
	friend void FPSSystem::FPSUpdatePhase(World*);
	friend float FPSSystem::GetFPS(World*);
	public:
		DebugWorldComponent() {};

	private:
		FPSSystem::FPSData FPSData;
	};
}