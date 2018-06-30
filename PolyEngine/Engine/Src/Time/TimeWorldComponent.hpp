#pragma once

#include <chrono>
#include <unordered_map>
#include "Time/TimeSystem.hpp"
#include "Timer.hpp"
#include "ECS/ComponentBase.hpp"


namespace Poly
{
	class ENGINE_DLLEXPORT TimeWorldComponent : public ComponentBase
	{
		friend void TimeSystem::TimeUpdatePhase(World* world);
		friend void TimeSystem::RegisterTimer(World* world, size_t id, bool isPausable, double multiplier);
		friend double TimeSystem::GetTimerDeltaTime(World* world, size_t id);
		friend double TimeSystem::GetTimerDeltaTime(World* world, eEngineTimer timerType);
		friend double TimeSystem::GetTimerElapsedTime(World* world, size_t id);
		friend double TimeSystem::GetTimerElapsedTime(World* world, eEngineTimer timerType);
		friend double TimeSystem::GetTimerMultiplier(World* world, size_t id);
		friend double TimeSystem::GetTimerMultiplier(World* world, eEngineTimer timerType);
	public:	
		TimeWorldComponent();

		double GetSystemTime() const { return Timers.at((size_t) eEngineTimer::SYSTEM).GetTime(); };
		double GetGameplayTime() const { return Timers.at((size_t) eEngineTimer::GAMEPLAY).GetTime(); };

		bool IsPaused() const { return Paused; };

		void SetPaused(bool isPaused) { Paused = isPaused; };

	private:
		const std::chrono::milliseconds MIN_TIME_STEP_MS = std::chrono::milliseconds(60);

		std::chrono::steady_clock::time_point LastFrameTime;

		std::unordered_map<size_t, Timer> Timers;

		bool Paused = false;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, TimeWorldComponent)
}