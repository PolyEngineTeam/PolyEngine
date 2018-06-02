#pragma once

#include <Collections/Queue.hpp>

#include "ECS/ComponentBase.hpp"
#include "DeferredTaskBase.hpp"

namespace Poly
{
	namespace DeferredTaskSystem
	{
		void DeferredTaskPhase(World* w);
		template<typename T, typename ...Args> T* AddComponentImmediate(World* w, Entity* entity, Args && ...args);
	}

	class ENGINE_DLLEXPORT DeferredTaskWorldComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(DeferredTaskWorldComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY();
		}

		friend void DeferredTaskSystem::DeferredTaskPhase(World*);
		template<typename T, typename ...Args> friend T* DeferredTaskSystem::AddComponentImmediate(World* w, Entity* entity, Args && ...args);
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

	REGISTER_COMPONENT(WorldComponentsIDGroup, DeferredTaskWorldComponent)
}