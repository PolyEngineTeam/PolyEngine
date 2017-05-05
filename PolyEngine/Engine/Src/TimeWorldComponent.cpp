#include "EnginePCH.hpp"

#include "TimeWorldComponent.hpp"


//------------------------------------------------------------------------------
Poly::TimeWorldComponent::TimeWorldComponent()
{
	LastFrameTime = steady_clock::now();
	SystemTime = GameplayTime = duration<float>::zero();
	IsPaused = false;
}
