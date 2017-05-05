#pragma once

#include <chrono>
#include "TimeSystem.hpp"
#include "Timer.hpp"
#include "ComponentBase.hpp"

using std::chrono::steady_clock;
using std::chrono::milliseconds;

namespace Poly
{
	class ENGINE_DLLEXPORT TimeWorldComponent : public ComponentBase
	{
		friend void TimeSystem::TimeUpdatePhase(World * world);
	public:	
		TimeWorldComponent();
		~TimeWorldComponent();

		float GetDeltaTime() const { return DeltaTime; };
		float GetSystemTime() const { return SystemTime->GetTime(); };
		float GetGameplayTime() const { return GameplayTime->GetTime(); };

		void SetPaused(bool isPaused) { IsPaused = isPaused; };

	private:
		const milliseconds MIN_TIME_STEP_MS = milliseconds(60);

		steady_clock::time_point LastFrameTime;
		Timer* SystemTime;
		Timer* GameplayTime;

		float DeltaTime;

		bool IsPaused;

	};
}