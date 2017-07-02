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
		/// <summary>Destroys entity after the end of frame.</summary>
		/// <param name="world">Pointer to world entity is in.</summary>
		/// <param name="entityId">ID of the entity to be removed.</summary>
		void ENGINE_DLLEXPORT DestroyEntity(World* world, const UniqueID& entityId);

		/// <summary>Adds component to entity after the end of frame.</summary>
		/// <param name="world">Pointer to world entity is in.</summary>
		/// <param name="entityId">ID of the entity.</summary>
		template<typename T, typename ...Args> void AddComponent(World* world, const UniqueID & entityId, Args && ...args)
		{
			DeferredTaskWorldComponent* cmp = world->GetWorldComponent<DeferredTaskWorldComponent>();
			cmp->ScheduleTask(new AddComponentDeferredTask<T, typename std::conditional<!std::is_array<typename std::remove_reference<Args>::type>::value, Args, typename std::decay<Args>::type>::type...>(entityId, std::forward<Args>(args)...));
		}

		/// <summary>Removes component from entity after the end of frame.</summary>
		/// <param name="world">Pointer to world entity is in.</summary>
		/// <param name="entityId">ID of the entity.</summary>
		template<typename T> void RemoveComponent(World* world, const UniqueID& entityId)
		{
			DeferredTaskWorldComponent* cmp = world->GetWorldComponent<DeferredTaskWorldComponent>();
			world->GetComponent<T>(entityId)->SetFlags(eComponentBaseFlags::ABOUT_TO_BE_REMOVED);
			cmp->ScheduleTask(new RemoveComponentDeferredTask<T>(entityId));
		}

		// IMMEDIATE CALLS
		
		/// <summary>Creates entity immediately.</summary>
		/// <param name="world">Pointer to world to create entity in.</summary>
		UniqueID ENGINE_DLLEXPORT SpawnEntityImmediate(World* w);

		/// <summary>Destroys entity immediately.</summary>
		/// <param name="world">Pointer to world entity is in.</summary>
		/// <param name="entityId">ID of the entity.</summary>
		void ENGINE_DLLEXPORT DestroyEntityImmediate(World* w, const UniqueID& entityId);

		/// <summary>Adds component to entity immediately.</summary>
		/// <param name="world">Pointer to world entity is in.</summary>
		/// <param name="entityId">ID of the entity.</summary>
		template<typename T, typename ...Args> void AddComponentImmediate(World* w, const UniqueID & entityId, Args && ...args)
		{
			w->AddComponent<T>(entityId, std::forward<Args>(args)...);
			DeferredTaskWorldComponent* cmp = w->GetWorldComponent<DeferredTaskWorldComponent>();
			T* newCmp = w->GetComponent<T>(entityId);
			cmp->NewlyCreatedComponents.PushBack(newCmp);
			newCmp->SetFlags(eComponentBaseFlags::NEWLY_CREATED);
		}

		/// <summary>Adds world component to world.</summary>
		/// <param name="world">Pointer to world.</summary>
		template<typename T, typename ...Args> void AddWorldComponentImmediate(World* w, Args && ...args)
		{
			w->AddWorldComponent<T>(std::forward<Args>(args)...);
		}

		/// <summary>Removes world component from world.</summary>
		/// <param name="world">Pointer to world.</summary>
		template<typename T> void RemoveWorldComponentImmediate(World* w)
		{
			DeferredTaskWorldComponent* cmp = w->GetWorldComponent<DeferredTaskWorldComponent>();
			w->RemoveWorldComponent<T>();
		}
	}
}
