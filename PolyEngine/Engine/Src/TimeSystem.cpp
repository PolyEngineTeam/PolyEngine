#include "EnginePCH.hpp"

#include "TimeSystem.hpp"
#include <chrono>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;

using namespace Poly;

//------------------------------------------------------------------------------
void TimeSystem::TimeUpdatePhase(World * world)
{
	TimeWorldComponent& timeComponent = world->GetTimeWorldComponent();

	//Get new delta time
	std::chrono::steady_clock::time_point frameTime = steady_clock::now();
	duration<double> deltaTime = duration_cast<duration<double>>(frameTime - timeComponent.LastFrameTime);

	//In future this should be surrounded by #ifndef FINAL
	deltaTime = deltaTime > duration_cast<duration<double>>(timeComponent.MIN_TIME_STEP_MS) ? timeComponent.MIN_TIME_STEP_MS : deltaTime;
	//#endif

	timeComponent.LastFrameTime = frameTime;

	//Update timers
	for (auto& timer : timeComponent.Timers)
	{
		if (timeComponent.Paused && timer.second.IsPausable)
		{
			timer.second.DeltaTime = 0.0;
			continue;
		}
		deltaTime *= timer.second.Multiplier;
		timer.second.Time += deltaTime;
		timer.second.DeltaTime = deltaTime.count();
	}
}

//------------------------------------------------------------------------------
void TimeSystem::RegisterTimer(World * world, size_t id, bool isPausable, double multiplier)
{
	TimeWorldComponent& timeComponent = world->GetTimeWorldComponent();
	ASSERTE(timeComponent.Timers.count(id) == 0, "Same timer id provided twice!");
	timeComponent.Timers[id] = Timer(isPausable, multiplier);
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerDeltaTime(World * world, size_t id)
{
	TimeWorldComponent& timeComponent = world->GetTimeWorldComponent();
	return timeComponent.Timers.count(id) == 0 ? 0.0f : timeComponent.Timers[id].GetDeltaTime();
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerDeltaTime(World * world, eEngineTimer timerType)
{
	return world->GetTimeWorldComponent().Timers.at((size_t)timerType).GetDeltaTime();
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerElapsedTime(World * world, size_t id)
{
	TimeWorldComponent& timeComponent = world->GetTimeWorldComponent();
	return timeComponent.Timers.count(id) == 0 ? 0.0f : timeComponent.Timers[id].GetTime();
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerElapsedTime(World * world, eEngineTimer timerType)
{
	return world->GetTimeWorldComponent().Timers.at((size_t)timerType).GetTime();
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerMultiplier(World * world, size_t id)
{
	TimeWorldComponent& timeComponent = world->GetTimeWorldComponent();
	return timeComponent.Timers.count(id) == 0 ? 0.0f : timeComponent.Timers[id].GetMultiplier();
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerMultiplier(World * world, eEngineTimer timerType)
{
	return world->GetTimeWorldComponent().Timers.at((size_t)timerType).GetMultiplier();
}
