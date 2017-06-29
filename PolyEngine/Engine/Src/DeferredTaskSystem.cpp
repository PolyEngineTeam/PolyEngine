#include "EnginePCH.hpp"
#include "DeferredTaskSystem.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
UniqueID DeferredTaskSystem::SpawnEntityImmediate(World* w)
{
	return w->SpawnEntity();
}

//------------------------------------------------------------------------------
void DeferredTaskSystem::DestroyEntityImmediate(World* w, const UniqueID& entityId)
{
	w->DestroyEntity(entityId);
}

//------------------------------------------------------------------------------
void DeferredTaskSystem::DestroyEntity(World* w, const UniqueID& entityId)
{
	DeferredTaskWorldComponent* cmp = w->GetWorldComponent<DeferredTaskWorldComponent>();
	cmp->ScheduleTask(new DestroyEntityDeferredTask(entityId));
}

//------------------------------------------------------------------------------
void DeferredTaskSystem::DeferredTaskPhase(World* w)
{
	DeferredTaskWorldComponent* cmp = w->GetWorldComponent<DeferredTaskWorldComponent>();

	// clear newly created flags after one tic.
	for (ComponentBase* cmp : cmp->NewlyCreatedComponents)
		cmp->ResetFlags(eComponentBaseFlags::NEWLY_CREATED);
	cmp->NewlyCreatedComponents.Clear();

	// execute tasks from queue
	while (!cmp->TasksQueue.IsEmpty())
	{
		DeferredTaskBase *task = cmp->TasksQueue.Front();
		ASSERTE(task, "The task doesn't exist!");

		gConsole.LogDebug("Executing task: {}", task->GetDescription());
		task->Execute(w);

		cmp->TasksQueue.PopFront();
		delete task;
	}
}

