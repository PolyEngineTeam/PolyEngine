#include "EnginePCH.hpp"
#include "HeavyTaskSystem.hpp"

using namespace Poly;

void HeavyTaskSystem::HeavyTaskPhase(World* w)
{
	HeavyTaskQueue& queue = w->GetHeavyTaskQueue();

	while (!queue.IsEmpty())
	{
		HeavyTaskBase *task = queue.Front();
		ASSERTE(task, "The task doesn't exist!");

		gConsole.LogDebug("Executing task: {}", task->GetDescription());
		task->Execute(w);

		queue.PopFront();
		delete task;
	}
}
