#include "EnginePCH.hpp"
#include "TimeSystem.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Poly::TimeSystem::TimeSystem()
{
	WorldStartTime = LastFrameTime = Clock.now();
	Timer = PausableTimer = std::chrono::duration<float>::zero();
	Paused = false;
}

//------------------------------------------------------------------------------
float TimeSystem::Update()
{
	std::chrono::steady_clock::time_point frameTime = Clock.now();
	DeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(frameTime - LastFrameTime);
#ifdef DEBUG
	const std::chrono::milliseconds MIN_TIME_STEP_MS = std::chrono::milliseconds(60);
	deltaTime = deltaTime > std::chrono::duration_cast<std::chrono::duration<float>>(MIN_TIME_STEP_MS) ? MIN_TIME_STEP_MS : deltaTime;
#endif // DEBUG
	LastFrameTime = frameTime;
	Timer += DeltaTime;
	if (!Paused)
	{
		PausableTimer += DeltaTime;
		return DeltaTime.count();
	}
	else
	{
		return 0.0f;
	}
}


