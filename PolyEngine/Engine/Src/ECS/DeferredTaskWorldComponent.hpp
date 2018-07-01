#pragma once

#include <Collections/Queue.hpp>

#include "ECS/ComponentBase.hpp"
#include "DeferredTaskBase.hpp"

namespace Poly
{
	namespace DeferredTaskSystem
	{
		void DeferredTaskPhase(Scene* w);
		template<typename T, typename ...Args> T* AddComponentImmediate(Scene* w, Entity* entity, Args && ...args);
	}

	class ENGINE_DLLEXPORT DeferredTaskWorldComponent : public ComponentBase
	{
		friend void DeferredTaskSystem::DeferredTaskPhase(Scene*);
		template<typename T, typename ...Args> friend T* DeferredTaskSystem::AddComponentImmediate(Scene* w, Entity* entity, Args && ...args);
	public:
		DeferredTaskWorldComponent() = default;

		void ScheduleTask(DeferredTaskBase* task) {
			TasksQueue.PushBack(task);
			//gConsole.LogDebug("New task scheduled: {}", task->GetDescription());
		}
	private:
		Queue<DeferredTaskBase*> TasksQueue;
		Dynarray<ComponentBase*> NewlyCreatedComponents;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, DeferredTaskWorldComponent)
}