#include "EnginePCH.hpp"
#include "TimeSystem.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Poly::TimeSystem::TimeSystem()
{
	WorldStartTime = LastFrameTime = Clock.now();
	Timer = PausableTimer = duration<float>::zero();
	Paused = false;
}

//------------------------------------------------------------------------------
float TimeSystem::Update()
{
	steady_clock::time_point frameTime = Clock.now();
	duration<float> deltaTime = duration_cast<duration<float>>(frameTime - LastFrameTime);
	LastFrameTime = frameTime;
	Timer += deltaTime;
	if (Paused)
	{
		PausableTimer += deltaTime;
		return deltaTime.count();
	}
	else
	{
		return 0.0f;
	}
}


