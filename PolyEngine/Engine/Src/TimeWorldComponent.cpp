#include "EnginePCH.hpp"

#include "TimeWorldComponent.hpp"


//------------------------------------------------------------------------------
Poly::TimeWorldComponent::TimeWorldComponent()
{
	LastFrameTime = steady_clock::now();
	SystemTime = Timer(false);
	GameplayTime = Timer(true);
}

