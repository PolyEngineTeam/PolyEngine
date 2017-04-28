#include "TimeSystem.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Poly::TimeSystem::TimeSystem()
{
	WorldStartTime = LastFrameTime = Clock.now();
	Timer = PausableTimer = duration<float>::zero();
}
//------------------------------------------------------------------------------
float TimeSystem::Update()
{
	steady_clock::time_point frameTime = Clock.now();
	duration<float> deltaTime = duration_cast<duration<float>>(frameTime - LastFrameTime);
#ifdef DEBUG
	const milliseconds MIN_TIME_STEP_MS = milliseconds(60);
	deltaTime = deltaTime > duration_cast<duration<float>>(MIN_TIME_STEP_MS) ? MIN_TIME_STEP_MS : deltaTime;
#endif // DEBUG
	LastFrameTime = frameTime;
	Timer += deltaTime;
	//TODO: Add Timer Pausing functionality
	if (1 /*!isGamePaused*/)
		PausableTimer += deltaTime;

	return deltaTime.count();
}


