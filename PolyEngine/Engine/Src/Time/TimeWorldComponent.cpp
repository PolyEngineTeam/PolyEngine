#include "EnginePCH.hpp"

#include "Time/TimeWorldComponent.hpp"


//------------------------------------------------------------------------------
Poly::TimeWorldComponent::TimeWorldComponent()
{
	LastFrameTime = std::chrono::steady_clock::now();
	Timers[(size_t) eEngineTimer::SYSTEM] = Timer(false);
	Timers[(size_t) eEngineTimer::GAMEPLAY] = Timer(true);
}

