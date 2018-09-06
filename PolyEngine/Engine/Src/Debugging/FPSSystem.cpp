#include "EnginePCH.hpp"

#include "Debugging/FPSSystem.hpp"
#include "Debugging/DebugDrawSystem.hpp"
#include "Debugging/DebugWorldComponent.hpp"
#include "Time/TimeSystem.hpp"
#include "ECS/DeferredTaskSystem.hpp"
#include "Configs/DebugConfig.hpp"

using namespace Poly;

void FPSSystem::FPSUpdatePhase(Scene* world)
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

	DebugDrawSystem::DrawText2D(world, Vector2i(50, 50), com->FPSData.DisplayedFPSText, 32);
}

float FPSSystem::GetFPS(Scene * world)
{
	return (float)world->GetWorldComponent<DebugWorldComponent>()->FPSData.LastFPS;
}
