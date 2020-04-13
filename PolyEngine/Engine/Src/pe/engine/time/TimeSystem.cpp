#include <pe/engine/EnginePCH.hpp>

#include <pe/engine/time/TimeSystem.hpp>
#include <pe/engine/time/TimeWorldComponent.hpp>
#include <pe/api/ecs/Scene.hpp>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;

namespace pe::engine::time
{

//------------------------------------------------------------------------------
void TimeSystem::onUpdate(::pe::api::ecs::Scene* scene)
{
	TimeWorldComponent* timeComponent = scene->getComponent<TimeWorldComponent>();

	//Get new delta time
	std::chrono::steady_clock::time_point frameTime = steady_clock::now();
	duration<double> deltaTime = duration_cast<duration<double>>(frameTime - timeComponent->LastFrameTime);

	//In future this should be surrounded by #ifndef FINAL
	deltaTime = deltaTime > duration_cast<duration<double>>(timeComponent->MIN_TIME_STEP_MS) ? timeComponent->MIN_TIME_STEP_MS : deltaTime;
	//#endif

	timeComponent->LastFrameTime = frameTime;

	//Update timers
	for (auto& timer : timeComponent->Timers)
	{
		if (timeComponent->Paused && timer.second.IsPausable)
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
void TimeSystem::RegisterTimer(api::ecs::Scene* scene, size_t id, bool isPausable, double multiplier)
{
	TimeWorldComponent* timeComponent = scene->getComponent<TimeWorldComponent>();
	ASSERTE(timeComponent->Timers.count(id) == 0, "Same timer id provided twice!");
	timeComponent->Timers[id] = Timer(isPausable, multiplier);
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerDeltaTime(api::ecs::Scene* scene, size_t id)
{
	TimeWorldComponent* timeComponent = scene->getComponent<TimeWorldComponent>();
	if(timeComponent->Timers.count(id) == 0)
		throw std::invalid_argument("Timer with given id does not exist.");
	else 
		return timeComponent->Timers[id].GetDeltaTime();
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerDeltaTime(api::ecs::Scene* scene, eEngineTimer timerType)
{
	return scene->getComponent<TimeWorldComponent>()->Timers.at((size_t)timerType).GetDeltaTime();
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerElapsedTime(api::ecs::Scene* scene, size_t id)
{
	TimeWorldComponent* timeComponent = scene->getComponent<TimeWorldComponent>();
	if(timeComponent->Timers.count(id) == 0)
		throw std::invalid_argument("Timer with given id does not exist.");
	else 
		return timeComponent->Timers[id].GetTime();
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerElapsedTime(api::ecs::Scene* scene, eEngineTimer timerType)
{
	return scene->getComponent<TimeWorldComponent>()->Timers.at((size_t)timerType).GetTime();
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerMultiplier(api::ecs::Scene* scene, size_t id)
{
	TimeWorldComponent* timeComponent = scene->getComponent<TimeWorldComponent>();
	if(timeComponent->Timers.count(id) == 0)
		throw std::invalid_argument("Timer with given id does not exist.");
	else 
		return timeComponent->Timers[id].GetMultiplier();
}

//------------------------------------------------------------------------------
double TimeSystem::GetTimerMultiplier(api::ecs::Scene* scene, eEngineTimer timerType)
{
	return scene->getComponent<TimeWorldComponent>()->Timers.at((size_t)timerType).GetMultiplier();
}

}