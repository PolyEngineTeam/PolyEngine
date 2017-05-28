#include "EnginePCH.hpp"
#include "HeavyTaskSystem.hpp"

using namespace Poly;

void HeavyTaskSystem::HeavyTaskPhase(World* w)
{
	HeavyTaskQueue& queue = w->GetHeavyTaskQueue();

	while (!queue.IsEmpty())
	{
		HeavyTaskBase *task = queue.Front();

		if (task)
		{
			gConsole.LogDebug("Executing task: {}", task->GetDescription());
			task->Execute(w);
		}

		queue.PopFront();
		
		if (task)
			delete task;
	}
}