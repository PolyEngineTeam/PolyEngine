#pragma once

#include <Defines.hpp>
#include "ECS/ComponentBase.hpp"
#include "Debugging/FPSSystem.hpp"
#include "Time/TimeSystem.hpp"

namespace Poly
{
	class Scene;

	/// <summary>WorldComponent that holds data for debug system.</summary>
	class ENGINE_DLLEXPORT DebugWorldComponent : public ComponentBase
	{
		RTTI_DECLARE_COMPONENT(::Poly::DebugWorldComponent) { NO_RTTI_PROPERTY(); }

		friend void FPSSystem::FPSUpdatePhase(Scene*);
		friend float FPSSystem::GetFPS(Scene*);

	private:
		FPSSystem::FPSData FPSData;
	};
	REGISTER_COMPONENT(ComponentsIDGroup, DebugWorldComponent)
}