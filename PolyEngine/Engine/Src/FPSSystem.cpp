#include "EnginePCH.hpp"

#include "FPSSystem.hpp"
#include "DeferredTaskSystem.hpp"

#include "DebugWorldComponent.hpp"
#include "TimeSystem.hpp"

using namespace Poly;

void FPSSystem::FPSUpdatePhase(World* world)
{
	DebugWorldComponent* com = world->GetWorldComponent<DebugWorldComponent>();

	if (gDebugConfig.DisplayFPS && !com->FPSData.TextID)
	{
		UniqueID id = DeferredTaskSystem::SpawnEntityImmediate(world);
		DeferredTaskSystem::AddComponentImmediate<ScreenSpaceTextComponent>(world,  id, Vector(300, 300, 0), "Fonts/Raleway/Raleway-Regular.ttf", eResourceSource::ENGINE, 32);
		com->FPSData.TextID = id;
	}
		
	
	if (gDebugConfig.DisplayFPS && TimeSystem::GetTimerElapsedTime(world, eEngineTimer::SYSTEM) - com->FPSData.ElapsedTime > 1)
	{
		com->FPSData.ElapsedTime = TimeSystem::GetTimerElapsedTime(world, eEngineTimer::SYSTEM);

		ScreenSpaceTextComponent* textCom = world->GetComponent<ScreenSpaceTextComponent>(com->FPSData.TextID);
		textCom->SetText(&std::string("FPS: " + std::to_string(com->FPSData.FPS))[0]);
		com->FPSData.FPS = 0;
	}

	com->FPSData.FPS++;
}

float FPSSystem::GetFPS(World * world)
{
	return (float)world->GetWorldComponent<DebugWorldComponent>()->FPSData.FPS;
}
