#include "EnginePCH.hpp"
#include "HeavyTaskSystem.hpp"

using namespace Poly;

void HeavyTaskSystem::HeavyTaskPhase(World* w)
{
	HeavyTaskQueue& queue = w->GetHeavyTaskQueue();

	while (!queue.Empty())
	{
		//todo - scan the queue and if object is deleted, delete all other tasks associated with this object
		//todo - private AddComp, RemoveComp, DestroyEntity in world.hpp
		HeavyTaskBase *task = queue.Front();

		task->Execute(w);

		queue.Pop();
		delete task;
	}
}