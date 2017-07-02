#include "EnginePCH.hpp"

#include "FPSSystem.hpp"
#include "DeferredTaskSystem.hpp"

#include "DebugWorldComponent.hpp"
#include "TimeSystem.hpp"

using namespace Poly;

void FPSSystem::FPSUpdatePhase(World* world)
{
	DebugWorldComponent* com = world->GetWorldComponent<DebugWorldComponent>();

	if (gCoreConfig.DisplayFPS && !com->FPSData.DisplayingFPS)
	{
		UniqueID id = DeferredTaskSystem::SpawnEntityImmediate(world);
		DeferredTaskSystem::AddComponentImmediate<ScreenSpaceTextComponent>(world,  id, Vector(0, 10, 0), "Fonts/Raleway/Raleway-Regular.ttf", 32);
		com->FPSData.DisplayingFPS = true;
	}
		
	
	if (TimeSystem::GetTimerElapsedTime(world, eEngineTimer::SYSTEM) - com->FPSData.ElapsedTime > 1)
	{
		com->FPSData.ElapsedTime = TimeSystem::GetTimerElapsedTime(world, eEngineTimer::SYSTEM);

		ScreenSpaceTextComponent* textCom;
		for (auto tuple : world->IterateComponents<ScreenSpaceTextComponent>())
		{
				textCom = std::get<ScreenSpaceTextComponent*>(tuple);
				textCom->SetText(&std::string("FPS: " + std::to_string(com->FPSData.FPS))[0]);
		}

		com->FPSData.FPS = 0;
	}

	com->FPSData.FPS++;
}

float FPSSystem::GetFPS(World * world)
{
	return world->GetWorldComponent<DebugWorldComponent>()->FPSData.FPS;
}
