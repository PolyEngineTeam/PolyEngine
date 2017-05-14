#include "EnginePCH.hpp"

#include "TimeSystem.hpp"
#include <chrono>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;

//------------------------------------------------------------------------------
void Poly::TimeSystem::TimeUpdatePhase(World * world)
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
		if (timeComponent.IsPaused && timer.second.IsPausable)
		{
			timer.second.DeltaTime = 0.0;
			continue;
		}
		timer.second.Time += deltaTime;
		timer.second.DeltaTime = deltaTime.count();
	}
}

//------------------------------------------------------------------------------
void Poly::TimeSystem::RegisterTimer(World * world, size_t timerId, bool isPausable)
{
	TimeWorldComponent& timeComponent = world->GetTimeWorldComponent();
	ASSERTE(timeComponent.Timers.count(timerId) == 0, "Same timer id provided twice!");
	timeComponent.Timers[timerId] = Timer(isPausable);
}

//------------------------------------------------------------------------------
double Poly::TimeSystem::GetTimerDeltaTime(World * world, size_t timerId)
{
	return world->GetTimeWorldComponent().Timers.at(timerId).GetDeltaTime();
}

//------------------------------------------------------------------------------
double Poly::TimeSystem::GetTimerDeltaTime(World * world, eEngineTimer timerType)
{
	return world->GetTimeWorldComponent().Timers.at((size_t)timerType).GetDeltaTime();
}

//------------------------------------------------------------------------------
double Poly::TimeSystem::GetTimerElapsedTime(World * world, size_t id)
{
	TimeWorldComponent& timeComponent = world->GetTimeWorldComponent();
	return timeComponent.Timers.count(id) == 0 ? 0.0f : timeComponent.Timers[id].GetTime();
}
