#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <ECS/DeferredTaskBase.hpp>

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
		RTTI_DECLARE_COMPONENT(::Poly::DeferredTaskWorldComponent) { NO_RTTI_PROPERTY(); }

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