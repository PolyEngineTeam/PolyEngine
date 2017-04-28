#pragma once

#include <chrono>
#include <Core.hpp>


namespace Poly
{
	class ENGINE_DLLEXPORT TimeSystem : public BaseObject<>
	{
	public:

		TimeSystem();
		~TimeSystem() {};

		float Update();
		float GetDeltaTime() const { return DeltaTime.count(); };
		float GetPausableTimer() const { return PausableTimer.count(); };
		float GetTimer() const { return Timer.count(); }

	private:
		std::chrono::steady_clock Clock;
		std::chrono::steady_clock::time_point WorldStartTime;
		std::chrono::steady_clock::time_point LastFrameTime;
		std::chrono::duration<float> DeltaTime;
		std::chrono::duration<float> PausableTimer;
		std::chrono::duration<float> Timer;
		bool Paused;
	};
}
