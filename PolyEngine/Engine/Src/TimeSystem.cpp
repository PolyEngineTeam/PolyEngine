#include "EnginePCH.hpp"

#include "TimeSystem.hpp"
#include <chrono>

void Poly::TimeSystem::TimeUpdatePhase(World * world)
{
	TimeWorldComponent timeComponent = world->GetTimeWorldComponent();
	std::chrono::steady_clock::time_point frameTime = std::chrono::steady_clock::now();
	timeComponent.DeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(frameTime - timeComponent.LastFrameTime);
#ifdef DEBUG
	timeComponent.DeltaTime = timeComponent.DeltaTime > std::chrono::duration_cast<std::chrono::duration<float>>(timeComponent.MIN_TIME_STEP_MS) ? timeComponent.MIN_TIME_STEP_MS : timeComponent.DeltaTime;
#endif // DEBUG
	timeComponent.LastFrameTime = frameTime;
	timeComponent.AbsoluteTime += timeComponent.DeltaTime;
	if (!timeComponent.IsPaused)
	{
		timeComponent.PausableTime += timeComponent.DeltaTime;
	}
}
