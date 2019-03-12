#pragma once

#include <Defines.hpp>

namespace Poly
{
	class Scene;
	enum class eEngineTimer { SYSTEM, GAMEPLAY, _COUNT };

	namespace TimeSystem
	{
		void TimeUpdatePhase(Scene* world);

		void ENGINE_DLLEXPORT RegisterTimer(Scene* world, size_t id, bool isPausable = false, double multiplier = 1);

		double ENGINE_DLLEXPORT GetTimerDeltaTime(Scene* world, size_t timerId);
		double ENGINE_DLLEXPORT GetTimerDeltaTime(Scene* world, eEngineTimer timerType);

		double ENGINE_DLLEXPORT GetTimerElapsedTime(Scene* world, size_t timerId);
		double ENGINE_DLLEXPORT GetTimerElapsedTime(Scene* world, eEngineTimer timerType);

		double ENGINE_DLLEXPORT GetTimerMultiplier(Scene* world, size_t timerId);
		double ENGINE_DLLEXPORT GetTimerMultiplier(Scene* world, eEngineTimer timerType);
	}
}