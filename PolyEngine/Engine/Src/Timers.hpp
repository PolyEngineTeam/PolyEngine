#pragma once

#include <chrono>
#include <Core.hpp>

namespace Poly
{

	class ENGINE_DLLEXPORT Timer : public BaseObject<>
	{
	public:
		Timer();
		~Timer() {};

		virtual float Update();
		float GetDeltaTime() const { return DeltaTime.count(); };
		float GetTimer() const { return Time.count(); }

	protected:
		void GetNextTimeStep();

		const std::chrono::milliseconds MIN_TIME_STEP_MS = std::chrono::milliseconds(60);

		std::chrono::steady_clock Clock;
		std::chrono::steady_clock::time_point WorldStartTime;
		std::chrono::steady_clock::time_point LastFrameTime;
		std::chrono::duration<float> DeltaTime;
		std::chrono::duration<float> Time;
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT PausableTimer : public Timer
	{
	public: 
		PausableTimer();
		~PausableTimer() {};

		void Pause() { Paused = true; };
		void Unpause() { Paused = false; };
		bool IsPaused() const { return Paused; };

		virtual float Update();

	private:
		bool Paused;
	};

}
