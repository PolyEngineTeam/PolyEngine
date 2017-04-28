#pragma once

#include <chrono>
#include <Core.hpp>

using namespace std::chrono;

namespace Poly
{
	class ENGINE_DLLEXPORT TimeSystem : public BaseObject<>
	{
	public:

		TimeSystem();
		~TimeSystem() {};

		float Update();
		float GetPausableTimer() const { return PausableTimer.count(); };
		float GetTimer() const { return Timer.count(); }

	private:
		steady_clock Clock;
		steady_clock::time_point WorldStartTime;
		steady_clock::time_point LastFrameTime;
		duration<float> PausableTimer;
		duration<float> Timer;	
	};
}
