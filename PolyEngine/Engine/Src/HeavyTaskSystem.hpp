#pragma once

#include "HeavyTaskImplementation.hpp"
#include "World.hpp"

namespace Poly
{
	namespace HeavyTaskSystem
	{
		void HeavyTaskPhase(World* w);

		inline void DestroyEntity(World* w, const UniqueID& entityId)
		{
			HeavyTaskBase *task = new DestroyEntityHeavyTask(entityId);
			w->GetHeavyTaskQueue().Push(task);
		}

		template<typename T, typename ...Args>
		void AddComponent(World* w, const UniqueID & entityId, Args && ...args)
		{
			HeavyTaskBase *task = new AddComponentHeavyTask<T>(entityId, args...);
			w->GetHeavyTaskQueue().Push(task);
		}

		template<typename T>
		void RemoveComponent(World* w, const UniqueID & entityId)
		{
			HeavyTaskBase *task = new RemoveComponentHeavyTask<T>(entityId);
			w->GetHeavyTaskQueue().Push(task);
		}
	}
}
