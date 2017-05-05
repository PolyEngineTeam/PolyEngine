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

	//Get new delta time
	std::chrono::steady_clock::time_point frameTime = steady_clock::now();
	duration<float> deltaTime = duration_cast<duration<float>>(frameTime - timeComponent.LastFrameTime);
#ifdef DEBUG
	deltaTime = deltaTime > duration_cast<duration<float>>(timeComponent.MIN_TIME_STEP_MS) ? timeComponent.MIN_TIME_STEP_MS : deltaTime;
#endif // DEBUG
	timeComponent.DeltaTime = deltaTime.count();
	timeComponent.LastFrameTime = frameTime;

	//Update timers
	timeComponent.SystemTime.Time += deltaTime;
	if (!timeComponent.IsPaused)
	{
		timeComponent.GameplayTime.Time += deltaTime;
	}
	for (auto& timer : timeComponent.Timers)
	{
		if (timeComponent.IsPaused && timer.second.IsPausable)
			continue;
		timer.second.Time += deltaTime;
	}
}

//------------------------------------------------------------------------------
void Poly::TimeSystem::RegisterTimer(World * world, size_t id, bool isPausable)
{
	TimeWorldComponent timeComponent = world->GetTimeWorldComponent();
	//if timer with that id exists, do nothing (for now)
	if (timeComponent.Timers.count(id) == 0)
	{
		timeComponent.Timers[id] = Timer(isPausable);
	}
}

//------------------------------------------------------------------------------
float Poly::TimeSystem::GetDeltaTime(World * world)
{
	return world->GetTimeWorldComponent().GetDeltaTime();
}

//------------------------------------------------------------------------------
float Poly::TimeSystem::GetTimerTime(World * world, size_t id)
{
	TimeWorldComponent timeComponent = world->GetTimeWorldComponent();
	return timeComponent.Timers.count(id) == 0 ? 0.0f : timeComponent.Timers[id].GetTime();
}
