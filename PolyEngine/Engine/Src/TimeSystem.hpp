#pragma once


namespace Poly
{
	class World;
	enum class eEngineTimer { SYSTEM, GAMEPLAY, _COUNT };

	namespace TimeSystem
	{
		void TimeUpdatePhase(World* world);

		void ENGINE_DLLEXPORT RegisterTimer(World* world, size_t id, bool isPausable);

		double ENGINE_DLLEXPORT GetTimerDeltaTime(World* world, size_t timerId);

		double ENGINE_DLLEXPORT GetTimerDeltaTime(World* world, eEngineTimer timerType);

		double ENGINE_DLLEXPORT GetTimerElapsedTime(World* world, size_t timerId);
	}
}