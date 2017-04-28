#include "EnginePCH.hpp"
#include "Timers.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Poly::Timer::Timer()
{
	WorldStartTime = LastFrameTime = Clock.now();
	Time = std::chrono::duration<float>::zero();
}

//------------------------------------------------------------------------------
Poly::PausableTimer::PausableTimer()
{
	__super::Update();
	Paused = false;
}

//------------------------------------------------------------------------------
float Timer::Update()
{
	GetNextTimeStep();
	Time += DeltaTime;
	return DeltaTime.count();
}

//------------------------------------------------------------------------------
float Poly::PausableTimer::Update()
{
	GetNextTimeStep();
	if (!Paused)
	{
		Time += DeltaTime;
		return DeltaTime.count();
	}
	else
	{
		return 0.0f;
	}
}

//------------------------------------------------------------------------------
void Poly::Timer::GetNextTimeStep()
{
	std::chrono::steady_clock::time_point frameTime = Clock.now();
	DeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(frameTime - LastFrameTime);
#ifdef DEBUG
	deltaTime = deltaTime > std::chrono::duration_cast<std::chrono::duration<float>>(MIN_TIME_STEP_MS) ? MIN_TIME_STEP_MS : deltaTime;
#endif // DEBUG
	LastFrameTime = frameTime;
}


