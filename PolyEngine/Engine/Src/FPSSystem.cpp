#include "EnginePCH.hpp"

#include "FPSSystem.hpp"

#include "DebugWorldComponent.hpp"
#include "TimeSystem.hpp"

void Poly::FPSSystem::FPSUpdatePhase(World* world)
{
	DebugWorldComponent* com = world->GetWorldComponent<DebugWorldComponent>();
	
	com->FPSData.FPS = 1 / TimeSystem::GetTimerDeltaTime(world, eEngineTimer::SYSTEM);

	if (gCoreConfig.DisplayFPS && com->FPSData.FPS)
	{

	}
}

float Poly::FPSSystem::GetFPS(World * world)
{
	return world->GetWorldComponent<DebugWorldComponent>()->FPSData.FPS;
}
