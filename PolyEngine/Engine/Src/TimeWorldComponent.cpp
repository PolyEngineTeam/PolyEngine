#include "EnginePCH.hpp"

#include "TimeWorldComponent.hpp"


//------------------------------------------------------------------------------

Poly::TimeWorldComponent::TimeWorldComponent()
{
	LastFrameTime = std::chrono::steady_clock::now();
	AbsoluteTime = PausableTime = std::chrono::duration<float>::zero();
	IsPaused = false;
}
