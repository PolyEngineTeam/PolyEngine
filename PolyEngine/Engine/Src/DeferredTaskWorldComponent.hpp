#pragma once

#include <Queue.hpp>

#include "ComponentBase.hpp"
#include "DeferredTaskBase.hpp"

namespace Poly
{
	namespace DeferredTaskSystem
	{
		void DeferredTaskPhase(World* w);
		template<typename T, typename ...Args> void AddComponentImmediate(World* w, const UniqueID & entityId, Args && ...args);
	}

	class ENGINE_DLLEXPORT DeferredTaskWorldComponent : public ComponentBase
	{
		friend void DeferredTaskSystem::DeferredTaskPhase(World*);
		template<typename T, typename ...Args> friend void DeferredTaskSystem::AddComponentImmediate(World* w, const UniqueID & entityId, Args && ...args);
	public:
		DeferredTaskWorldComponent() = default;

		void ScheduleTask(DeferredTaskBase* task) {
			TasksQueue.PushBack(task);
			gConsole.LogDebug("New task scheduled: {}", task->GetDescription());
		}
	private:
		Queue<DeferredTaskBase*> TasksQueue;
		Dynarray<ComponentBase*> NewlyCreatedComponents;
	};
}