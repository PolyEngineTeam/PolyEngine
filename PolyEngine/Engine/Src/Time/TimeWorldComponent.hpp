#pragma once

#include <Defines.hpp>
#include <Time/TimeSystem.hpp>
#include <Time/Timer.hpp>
#include <ECS/ComponentBase.hpp>


namespace Poly
{
	class ENGINE_DLLEXPORT TimeWorldComponent : public ComponentBase
	{
		friend void TimeSystem::TimeUpdatePhase(Scene* world);
		friend void TimeSystem::RegisterTimer(Scene* world, size_t id, bool isPausable, double multiplier);
		friend double TimeSystem::GetTimerDeltaTime(Scene* world, size_t id);
		friend double TimeSystem::GetTimerDeltaTime(Scene* world, eEngineTimer timerType);
		friend double TimeSystem::GetTimerElapsedTime(Scene* world, size_t id);
		friend double TimeSystem::GetTimerElapsedTime(Scene* world, eEngineTimer timerType);
		friend double TimeSystem::GetTimerMultiplier(Scene* world, size_t id);
		friend double TimeSystem::GetTimerMultiplier(Scene* world, eEngineTimer timerType);
	public:
		RTTI_DECLARE_COMPONENT(::Poly::TimeWorldComponent) { NO_RTTI_PROPERTY(); }

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