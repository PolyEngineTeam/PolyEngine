#pragma once

#include <Defines.hpp>
#include <ECS/DeferredTaskImplementation.hpp>
#include <ECS/DeferredTaskWorldComponent.hpp>
#include <ECS/Scene.hpp>

namespace Poly
{
	namespace DeferredTaskSystem
	{
		void DeferredTaskPhase(Scene* w);

		// NORMAL CALLS
		/// <summary>Destroys entity after the end of frame.</summary>
		/// <param name="world">Pointer to world entity is in.</summary>
		/// <param name="entity">ID of the entity to be removed.</summary>
		void ENGINE_DLLEXPORT DestroyEntity(Scene* world, Entity* entity);

		/// <summary>Adds component to entity after the end of frame.</summary>
		/// <param name="world">Pointer to world entity is in.</summary>
		/// <param name="entity">ID of the entity.</summary>
		template<typename T, typename ...Args> void AddComponent(Scene* world, Entity* entity, Args && ...args)
		{
			DeferredTaskWorldComponent* cmp = world->GetWorldComponent<DeferredTaskWorldComponent>();
			cmp->ScheduleTask(new AddComponentDeferredTask<T, typename std::conditional<!std::is_array<typename std::remove_reference<Args>::type>::value, Args, typename std::decay<Args>::type>::type...>(entity, std::forward<Args>(args)...));
		}

		/// <summary>Removes component from entity after the end of frame.</summary>
		/// <param name="world">Pointer to world entity is in.</summary>
		/// <param name="entity">ID of the entity.</summary>
		template<typename T> void RemoveComponent(Scene* world, Entity* entity)
		{
			DeferredTaskWorldComponent* cmp = world->GetWorldComponent<DeferredTaskWorldComponent>();
			world->GetComponent<T>(entity)->SetFlags(eComponentBaseFlags::ABOUT_TO_BE_REMOVED);
			cmp->ScheduleTask(new RemoveComponentDeferredTask<T>(entity));
		}

		// IMMEDIATE CALLS

		/// <summary>Creates entity immediately.</summary>
		/// <param name="world">Pointer to world to create entity in.</summary>
		ENGINE_DLLEXPORT Entity* SpawnEntityImmediate(Scene* w);

		/// <summary>Destroys entity immediately.</summary>
		/// <param name="world">Pointer to world entity is in.</summary>
		/// <param name="entity">ID of the entity.</summary>
		ENGINE_DLLEXPORT void DestroyEntityImmediate(Scene* w, Entity* entity);

		/// <summary>Adds component to entity immediately.</summary>
		/// <param name="world">Pointer to world entity is in.</summary>
		/// <param name="entity">ID of the entity.</summary>
		template<typename T, typename ...Args> T* AddComponentImmediate(Scene* w, Entity* entity, Args && ...args)
		{
			w->AddComponent<T>(entity, std::forward<Args>(args)...);
			DeferredTaskWorldComponent* cmp = w->GetWorldComponent<DeferredTaskWorldComponent>();
			T* newCmp = w->GetComponent<T>(entity);
			cmp->NewlyCreatedComponents.PushBack(newCmp);
			newCmp->SetFlags(eComponentBaseFlags::NEWLY_CREATED);
			return newCmp;
		}

		/// <summary>Adds world component to world.</summary>
		/// <param name="world">Pointer to world.</summary>
		template<typename T, typename ...Args> T* AddWorldComponentImmediate(Scene* w, Args && ...args)
		{
			w->AddWorldComponent<T>(std::forward<Args>(args)...);
			return w->GetWorldComponent<T>();
		}

		/// <summary>Removes world component from world.</summary>
		/// <param name="world">Pointer to world.</summary>
		template<typename T> void RemoveWorldComponentImmediate(Scene* w)
		{
			w->RemoveWorldComponent<T>();
		}
	}
}
