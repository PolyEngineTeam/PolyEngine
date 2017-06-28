#pragma once

#include "DeferredTaskImplementation.hpp"
#include "DeferredTaskWorldComponent.hpp"
#include "World.hpp"

namespace Poly
{
	namespace DeferredTaskSystem
	{
		void DeferredTaskPhase(World* w);

		// NORMAL CALLS
		void ENGINE_DLLEXPORT DestroyEntity(World* w, const UniqueID& entityId);

		//------------------------------------------------------------------------------
		template<typename T, typename ...Args> void AddComponent(World* w, const UniqueID & entityId, Args && ...args)
		{
			DeferredTaskWorldComponent* cmp = w->GetWorldComponent<DeferredTaskWorldComponent>();
			cmp->ScheduleTask(new AddComponentDeferredTask<T, typename std::conditional<!std::is_array<typename std::remove_reference<Args>::type>::value, Args, typename std::decay<Args>::type>::type...>(entityId, std::forward<Args>(args)...));
		}

		//------------------------------------------------------------------------------
		template<typename T> void RemoveComponent(World* w, const UniqueID& entityId)
		{
			DeferredTaskWorldComponent* cmp = w->GetWorldComponent<DeferredTaskWorldComponent>();
			w->GetComponent<T>(entityId)->SetFlags(eComponentBaseFlags::ABOUT_TO_BE_REMOVED);
			cmp->ScheduleTask(new RemoveComponentDeferredTask<T>(entityId));
		}

		// IMMEDIATE CALLS
		UniqueID ENGINE_DLLEXPORT SpawnEntityImmediate(World* w);
		void ENGINE_DLLEXPORT DestroyEntityImmediate(World* w, const UniqueID& entityId);

		//------------------------------------------------------------------------------
		template<typename T, typename ...Args> void AddComponentImmediate(World* w, const UniqueID & entityId, Args && ...args)
		{
			w->AddComponent<T>(entityId, std::forward<Args>(args)...);
			DeferredTaskWorldComponent* cmp = w->GetWorldComponent<DeferredTaskWorldComponent>();
			T* newCmp = w->GetComponent<T>(entityId);
			cmp->NewlyCreatedComponents.PushBack(newCmp);
			newCmp->SetFlags(eComponentBaseFlags::NEWLY_CREATED);
		}

		//------------------------------------------------------------------------------
		template<typename T, typename ...Args> void AddWorldComponentImmediate(World* w, Args && ...args)
		{
			w->AddWorldComponent<T>(std::forward<Args>(args)...);
		}

		//------------------------------------------------------------------------------
		template<typename T> void RemoveWorldComponentImmediate(World* w)
		{
			DeferredTaskWorldComponent* cmp = w->GetWorldComponent<DeferredTaskWorldComponent>();
			w->RemoveWorldComponent<T>();
		}
	}
}
