#include "EnginePCH.hpp"
#include "DeferredTaskSystem.hpp"

using namespace Poly;

void DeferredTaskSystem::DeferredTaskPhase(World* w)
{
	DeferredTaskQueue& queue = w->GetDeferredTaskQueue();

	while (!queue.IsEmpty())
	{
		DeferredTaskBase *task = queue.Front();
		ASSERTE(task, "The task doesn't exist!");

		gConsole.LogDebug("Executing task: {}", task->GetDescription());
		task->Execute(w);

		queue.PopFront();
		delete task;
	}
}

UniqueID DeferredTaskSystem::SpawnEntityImmediate(World* w)
{
	/*SpawnEntityDeferredTask *task = new SpawnEntityDeferredTask();
	task->Execute(w);
	gConsole.LogDebug("New immediate task executed: {}", task->GetDescription());
	UniqueID ID = task->GetID();
	delete task;
	return ID;*/
	UniqueID id = w->SpawnEntity();
	gConsole.LogDebug("New immediate task executed: Spawn entity");
	return id;
}

void DeferredTaskSystem::DestroyEntity(World* w, const UniqueID& entityId)
{
	w->GetDeferredTaskQueue().PushBack(new DestroyEntityDeferredTask(entityId));
	gConsole.LogDebug("New task scheduled: {}", w->GetDeferredTaskQueue().Back()->GetDescription());
}

void DeferredTaskSystem::DestroyEntityImmediate(World* w, const UniqueID& entityId)
{
	/*DestroyEntityDeferredTask *task = new DestroyEntityDeferredTask(entityId);
	task->Execute(w);
	gConsole.LogDebug("New immediate task executed: {}", task->GetDescription());
	delete task;*/
	w->DestroyEntity(entityId);
	gConsole.LogDebug("New immediate task executed: Destroy entity");
}