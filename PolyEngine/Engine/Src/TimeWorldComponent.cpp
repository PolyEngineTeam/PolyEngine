#include "EnginePCH.hpp"

#include "TimeWorldComponent.hpp"


//------------------------------------------------------------------------------
void Poly::TimeWorldComponent::Update()
{
	DeltaTime = Time.Update();
}
