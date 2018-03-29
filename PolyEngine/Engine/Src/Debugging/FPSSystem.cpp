#include "EnginePCH.hpp"

#include "FPSSystem.hpp"
#include "DeferredTaskSystem.hpp"

#include "DebugWorldComponent.hpp"
#include "TimeSystem.hpp"
#include "DebugDrawSystem.hpp"

using namespace Poly;

void FPSSystem::FPSUpdatePhase(World* world)
{
	DebugWorldComponent* com = world->GetWorldComponent<DebugWorldComponent>();
	
	if (gDebugConfig.DisplayFPS && TimeSystem::GetTimerElapsedTime(world, eEngineTimer::SYSTEM) - com->FPSData.ElapsedTime > 1)
	{
		com->FPSData.ElapsedTime = TimeSystem::GetTimerElapsedTime(world, eEngineTimer::SYSTEM);

		StringBuilder sb;
		sb.AppendFormat("FPS: {}", com->FPSData.FPS);
		com->FPSData.DisplayedFPSText = sb.StealString();
		com->FPSData.LastFPS = com->FPSData.FPS;
		com->FPSData.FPS = 0;
	}

	com->FPSData.FPS++;

	DebugDrawSystem::DrawText2D(world, Vector2i(300, 300), com->FPSData.DisplayedFPSText, 32);
}

float FPSSystem::GetFPS(World * world)
{
	return (float)world->GetWorldComponent<DebugWorldComponent>()->FPSData.LastFPS;
}
