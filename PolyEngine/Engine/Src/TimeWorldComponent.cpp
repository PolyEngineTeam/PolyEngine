#include "EnginePCH.hpp"

#include "TimeWorldComponent.hpp"


//------------------------------------------------------------------------------
Poly::TimeWorldComponent::TimeWorldComponent()
{
	LastFrameTime = steady_clock::now();
	SystemTime = new Timer(false);
	GameplayTime = new Timer(true);
}

Poly::TimeWorldComponent::~TimeWorldComponent()
{
	delete SystemTime;
	delete GameplayTime;
}
