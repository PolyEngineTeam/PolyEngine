#pragma once

#include "HeavyTaskImplementation.hpp"
#include "World.hpp"

namespace Poly
{
	namespace HeavyTaskSystem
	{
		void HeavyTaskPhase(World* w)
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

		void DestroyEntity(World* w, const UniqueID& entityId)
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