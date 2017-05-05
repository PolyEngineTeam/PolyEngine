#include "EnginePCH.hpp"

#include "TimeSystem.hpp"
#include <chrono>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;

//------------------------------------------------------------------------------
void Poly::TimeSystem::TimeUpdatePhase(World * world)
{
	TimeWorldComponent timeComponent = world->GetTimeWorldComponent();
	std::chrono::steady_clock::time_point frameTime = steady_clock::now();
	duration<float> deltaTime = duration_cast<duration<float>>(frameTime - timeComponent.LastFrameTime);
#ifdef DEBUG
	deltaTime = deltaTime > duration_cast<duration<float>>(timeComponent.MIN_TIME_STEP_MS) ? timeComponent.MIN_TIME_STEP_MS : deltaTime;
#endif // DEBUG
	timeComponent.DeltaTime = deltaTime.count();
	timeComponent.LastFrameTime = frameTime;
	timeComponent.SystemTime->Time += deltaTime;
	if (!timeComponent.IsPaused)
	{
		timeComponent.GameplayTime->Time += deltaTime;
	}
}
