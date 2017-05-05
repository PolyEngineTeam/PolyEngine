#include "EnginePCH.hpp"

#include "TimeSystem.hpp"
#include <chrono>

void Poly::TimeSystem::TimeUpdatePhase(World * world)
{
	TimeWorldComponent timeComponent = world->GetTimeWorldComponent();
	std::chrono::steady_clock::time_point frameTime = steady_clock::now();
	timeComponent.DeltaTime = duration_cast<duration<float>>(frameTime - timeComponent.LastFrameTime);
#ifdef DEBUG
	timeComponent.DeltaTime = timeComponent.DeltaTime > duration_cast<duration<float>>(timeComponent.MIN_TIME_STEP_MS) ? timeComponent.MIN_TIME_STEP_MS : timeComponent.DeltaTime;
#endif // DEBUG
	timeComponent.LastFrameTime = frameTime;
	timeComponent.SystemTime += timeComponent.DeltaTime;
	if (!timeComponent.IsPaused)
	{
		timeComponent.GameplayTime += timeComponent.DeltaTime;
	}
}
