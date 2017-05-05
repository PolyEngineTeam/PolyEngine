#pragma once

#include <chrono>
#include "TimeSystem.hpp"
#include "ComponentBase.hpp"

using std::chrono::duration;
using std::chrono::steady_clock;
using std::chrono::duration_cast;

namespace Poly
{
	class ENGINE_DLLEXPORT TimeWorldComponent : public ComponentBase
	{
		friend void TimeSystem::TimeUpdatePhase(World * world);
	public:	
		TimeWorldComponent();
		~TimeWorldComponent() {};

		float GetDeltaTime() const { return DeltaTime.count(); };
		float GetSystemTime() const { return SystemTime.count(); };
		float GetGameplayTime() const { return GameplayTime.count(); };

		void SetPaused(bool isPaused) { IsPaused = isPaused; };

	private:
		const std::chrono::milliseconds MIN_TIME_STEP_MS = std::chrono::milliseconds(60);

		steady_clock::time_point LastFrameTime;
		duration<float> DeltaTime;
		duration<float> SystemTime;
		duration<float> GameplayTime;

		bool IsPaused;

	};
}