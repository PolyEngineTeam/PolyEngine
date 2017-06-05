#pragma once

#include "DeferredTaskImplementation.hpp"
#include "World.hpp"

namespace Poly
{
	namespace DeferredTaskSystem
	{
		void DeferredTaskPhase(World* w);

		UniqueID ENGINE_DLLEXPORT SpawnEntityImmediate(World* w);
		void ENGINE_DLLEXPORT DestroyEntity(World* w, const UniqueID& entityId);
		void ENGINE_DLLEXPORT DestroyEntityImmediate(World* w, const UniqueID& entityId);

		//------------------------------------------------------------------------------
		template<typename T, typename ...Args> void AddComponent(World* w, const UniqueID & entityId, Args && ...args)
		{
			w->GetDeferredTaskQueue().PushBack(new AddComponentDeferredTask<T, typename std::conditional<!std::is_array<typename std::remove_reference<Args>::type>::value, Args, typename std::decay<Args>::type>::type...>(entityId, std::forward<Args>(args)...));
			gConsole.LogDebug("New task scheduled: {}", w->GetDeferredTaskQueue().Back()->GetDescription());
		}

		//------------------------------------------------------------------------------
		template<typename T, typename ...Args> void AddComponentImmediate(World* w, const UniqueID & entityId, Args && ...args)
		{
			w->AddComponent<T>(entityId, std::forward<Args>(args)...);
			gConsole.LogDebug("New immediate task executed: Add component");
		}

		//------------------------------------------------------------------------------
		template<typename T> void RemoveComponent(World* w, const UniqueID & entityId)
		{
			w->GetDeferredTaskQueue().PushBack(new RemoveComponentDeferredTask<T>(entityId));
			gConsole.LogDebug("New task scheduled: {}", w->GetDeferredTaskQueue().Back()->GetDescription());
		}

		//------------------------------------------------------------------------------
		template<typename T, typename ...Args> void AddWorldComponent(World* w, Args && ...args)
		{
			w->GetDeferredTaskQueue().PushBack(new AddWorldComponentDeferredTask<T, typename std::conditional<!std::is_array<typename std::remove_reference<Args>::type>::value, Args, typename std::decay<Args>::type>::type...>(std::forward<Args>(args)...));
			gConsole.LogDebug("New task scheduled: {}", w->GetDeferredTaskQueue().Back()->GetDescription());
		}

		//------------------------------------------------------------------------------
		template<typename T, typename ...Args> void AddWorldComponentImmediate(World* w, Args && ...args)
		{
			w->AddWorldComponent<T>(std::forward<Args>(args)...);
			gConsole.LogDebug("New immediate task executed: Add world component");
		}

		//------------------------------------------------------------------------------
		template<typename T> void RemoveWorldComponent(World* w)
		{
			w->GetDeferredTaskQueue().PushBack(new RemoveWorldComponentDeferredTask<T>());
			gConsole.LogDebug("New task scheduled: {}", w->GetDeferredTaskQueue().Back()->GetDescription());
		}
	}
}
