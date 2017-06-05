#pragma once

#include "ComponentBase.hpp"
#include "FPSSystem.hpp"

namespace Poly
{
	class World;

	namespace FPSSystem
	{
		struct FPSData
		{
			double FPS;
			bool displayingFPS;
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