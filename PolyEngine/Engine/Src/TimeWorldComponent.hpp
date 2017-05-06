#pragma once

#include <chrono>
#include <unordered_map>
#include "TimeSystem.hpp"
#include "Timer.hpp"
#include "ComponentBase.hpp"

using std::chrono::steady_clock;
using std::chrono::milliseconds;

namespace Poly
{
	class ENGINE_DLLEXPORT TimeWorldComponent : public ComponentBase
	{
		friend void TimeSystem::TimeUpdatePhase(World* world);
		friend void TimeSystem::RegisterTimer(World* world, const size_t id, const bool isPausable);
		friend float TimeSystem::GetTimerTime(World* world, const size_t id);
	public:	
		TimeWorldComponent();
		~TimeWorldComponent() {};

		float GetDeltaTime() const { return DeltaTime; };
		float GetSystemTime() const { return SystemTime.GetTime(); };
		float GetGameplayTime() const { return GameplayTime.GetTime(); };

		void SetPaused(bool isPaused) { IsPaused = isPaused; };

	private:
		const milliseconds MIN_TIME_STEP_MS = milliseconds(60);

		steady_clock::time_point LastFrameTime;

		Timer SystemTime;
		Timer GameplayTime;

		std::unordered_map<size_t, Timer> Timers;

		float DeltaTime;

		bool IsPaused;
	};
}