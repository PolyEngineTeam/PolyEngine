#pragma once

#include <chrono>
#include "TimeSystem.hpp"
#include "ComponentBase.hpp"



namespace Poly
{
	class ENGINE_DLLEXPORT TimeWorldComponent : public ComponentBase
	{
		friend void TimeSystem::TimeUpdatePhase(World * world);
	public:	
		TimeWorldComponent();
		~TimeWorldComponent() {};

		float GetDeltaTime() const { return DeltaTime.count(); };
		float GetAbsoluteTime() const { return AbsoluteTime.count(); };
		float GetPausableTime() const { return PausableTime.count(); };

		void SetPaused(bool isPaused) { IsPaused = isPaused; };

	private:
		const std::chrono::milliseconds MIN_TIME_STEP_MS = std::chrono::milliseconds(60);

		std::chrono::steady_clock::time_point LastFrameTime;
		std::chrono::duration<float> DeltaTime;
		std::chrono::duration<float> AbsoluteTime;
		std::chrono::duration<float> PausableTime;

		bool IsPaused;

	};
}