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
		float GetPausableTimer() { return PausableTimer.count(); };
		float GetTimer() { return Timer.count(); }

	private:
		steady_clock Clock;
		steady_clock::time_point WorldStartTime;
		steady_clock::time_point LastFrameTime;
		duration<float> PausableTimer;
		duration<float> Timer;	
	};
}
