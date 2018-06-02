#pragma once

#include "ECS/ComponentBase.hpp"
#include "FPSSystem.hpp"
#include "Time/TimeSystem.hpp"

namespace Poly
{
	class World;

	/// <summary>WorldComponent that holds data for debug system.</summary>
	class ENGINE_DLLEXPORT DebugWorldComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(DebugWorldComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY();
		}

		friend void FPSSystem::FPSUpdatePhase(World*);
		friend float FPSSystem::GetFPS(World*);

	private:
		FPSSystem::FPSData FPSData;
	};
	REGISTER_COMPONENT(WorldComponentsIDGroup, DebugWorldComponent)
}