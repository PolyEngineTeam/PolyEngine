#pragma once

#include <unordered_map>
#include <Core.hpp>

#include "Entity.hpp"
#include "Engine.hpp"

#include "InputWorldComponent.hpp"
#include "ViewportWorldComponent.hpp"
#include "TimeWorldComponent.hpp"

#include "DeferredTaskBase.hpp"

namespace Poly {

	namespace DeferredTaskSystem
	{
		UniqueID ENGINE_DLLEXPORT SpawnEntityImmediate(World* w);
		void ENGINE_DLLEXPORT DestroyEntityImmediate(World* w, const UniqueID& entityId);
		template<typename T, typename ...Args> void AddComponentImmediate(World* w, const UniqueID & entityId, Args && ...args);
		template<typename T, typename ...Args> void AddWorldComponentImmediate(World* w, Args && ...args);
	}

	constexpr size_t MAX_ENTITY_COUNT = 65536;
	constexpr size_t MAX_WORLD_COMPONENTS_COUNT = 64;

	struct InputState;

	class ENGINE_DLLEXPORT World : public BaseObject<>
	{
	public:
		World(Engine* engine);
		virtual ~World();

		//TODO implement world
		//------------------------------------------------------------------------------
		//////////////////////////////
		/// Gets a component of a specified type and UniqueID.
		/// @tparam T - component type to get
		/// @param entityId - UniqueID of a component to get
		/// @return pointer to a specified component or a nullptr, if none was found
		/// @see AddComponent()
		/// @see RemoveComponent()
		//////////////////////////////
		template<typename T>
		T* GetComponent(const UniqueID& entityId)
		{
			HEAVY_ASSERTE(!!entityId, "Invalid entity ID");
			auto iter = IDToEntityMap.find(entityId);
			HEAVY_ASSERTE(iter != IDToEntityMap.end(), "Invalid entityId - entity with that ID does not exist!");
			return iter->second->GetComponent<T>();
		}

		//------------------------------------------------------------------------------
		bool HasWorldComponent(size_t ID) const;

		//------------------------------------------------------------------------------
		template<typename T>
		T* GetWorldComponent()
		{			
			ASSERTE(HasWorldComponent(EnginePtr->GetWorldComponentID<T>()), "dupa");
			return reinterpret_cast<T*>(Components[EnginePtr->GetWorldComponentID<T>()]);
		}

		Engine* GetEngine() const { return EnginePtr; }

		template<typename PrimaryComponent, typename... SecondaryComponents>
		struct IteratorProxy;

		/**
		 * Allows iteration over multiple component types
		 * Iterator dereferences to a tuple of component pointers
		 *
		 * To get the component out of the tuple use std::get()
		 * e.g. `std::get<YourComponentType*>(components)`
		 * If you have a C++17-compliant compiler, you can use structured bindings
		 * e.g. `for(auto [a, b] : world->IterateComponents<ComponentA, ComponentB>())`
		 *
		 * @tparam PrimaryComponent At least one component type must be specified
		 * @tparam SecondaryComponents Additional component types (warning: returned pointers might be null!)
		 * @return A proxy object that can be used in a range-for loop.
		 */
		template<typename PrimaryComponent, typename... SecondaryComponents>
		IteratorProxy<PrimaryComponent, SecondaryComponents...> IterateComponents()
		{
			return {this};
		}

		//------------------------------------------------------------------------------
		/**
		 * An implementation detail
		 */
		template<typename PrimaryComponent, typename... SecondaryComponents>
		class ComponentIterator : public BaseObject<>,
		                          public std::iterator<std::bidirectional_iterator_tag, std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type...>>
		{
			public:
			bool operator==(const ComponentIterator& rhs) const { return primary_iter == rhs.primary_iter; }
			bool operator!=(const ComponentIterator& rhs) const { return !(*this == rhs); }

			std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type...> operator*() const
			{
				PrimaryComponent* primary = &*primary_iter;
				return std::make_tuple(primary, primary->template GetSibling<SecondaryComponents>()...);
			}
			std::tuple<typename std::add_pointer<PrimaryComponent>::type, typename std::add_pointer<SecondaryComponents>::type...> operator->() const
			{
				return **this;
			}

			ComponentIterator& operator++() { ++primary_iter; return *this; }
			ComponentIterator operator++(int) { ComponentIterator ret(primary_iter); ++primary_iter; return ret; }
			ComponentIterator& operator--() { --primary_iter; return *this; }
			ComponentIterator operator--(int) { ComponentIterator ret(primary_iter); --primary_iter; return ret; }

			private:
			explicit ComponentIterator(typename IterablePoolAllocator<PrimaryComponent>::Iterator parent) : primary_iter(parent) {}
			friend struct IteratorProxy<PrimaryComponent, SecondaryComponents...>;

			typename IterablePoolAllocator<PrimaryComponent>::Iterator primary_iter;
		};

		/**
		 * An implementation detail
		 */
		template<typename PrimaryComponent, typename... SecondaryComponents>
		struct IteratorProxy : BaseObject<>
		{
			IteratorProxy(World* w) : W(w) {}
			World::ComponentIterator<PrimaryComponent, SecondaryComponents...> Begin()
			{
				return ComponentIterator<PrimaryComponent, SecondaryComponents...>(W->GetComponentAllocator<PrimaryComponent>()->Begin());
			}
			World::ComponentIterator<PrimaryComponent, SecondaryComponents...> End()
			{
				return ComponentIterator<PrimaryComponent, SecondaryComponents...>(W->GetComponentAllocator<PrimaryComponent>()->End());
			}
			auto begin() { return Begin(); }
			auto end() { return End(); }
			World* const W;
		};

		DeferredTaskQueue& GetDeferredTaskQueue() { return DeferredTasksQueue; }

	private:
		friend class SpawnEntityDeferredTask;
		friend class DestroyEntityDeferredTask;
		template<typename T,typename... Args> friend class AddComponentDeferredTask;
		template<typename T> friend class RemoveComponentDeferredTask;
		template<typename T,typename... Args> friend class AddWorldComponentDeferredTask;
		template<typename T> friend class RemoveWorldComponentDeferredTask;

		friend UniqueID DeferredTaskSystem::SpawnEntityImmediate(World*);
		friend void DeferredTaskSystem::DestroyEntityImmediate(World* w, const UniqueID& entityId);
		template<typename T, typename ...Args> friend void DeferredTaskSystem::AddComponentImmediate(World* w, const UniqueID & entityId, Args && ...args);
		template<typename T, typename ...Args> friend void DeferredTaskSystem::AddWorldComponentImmediate(World* w, Args && ...args);

		//------------------------------------------------------------------------------
		UniqueID SpawnEntity();
		//------------------------------------------------------------------------------
		void DestroyEntity(const UniqueID& entityId);
		//------------------------------------------------------------------------------
		template<typename T, typename... Args>
		void AddComponent(const UniqueID& entityId, Args&&... args)
		{
			T* ptr = GetComponentAllocator<T>()->Alloc();
			::new(ptr) T(std::forward<Args>(args)...);
			Entity* ent = IDToEntityMap[entityId];
			HEAVY_ASSERTE(ent, "Invalid entity ID");
			HEAVY_ASSERTE(!ent->HasComponent(EnginePtr->GetComponentID<T>()), "Failed at AddComponent() - a component of a given UniqueID already exists!");
			ent->ComponentPosessionFlags.set(EnginePtr->GetComponentID<T>(), true);
			ent->Components[EnginePtr->GetComponentID<T>()] = ptr;
			ptr->Owner = ent;
		}

		//------------------------------------------------------------------------------
		template<typename T>
		void RemoveComponent(const UniqueID& entityId)
		{
			Entity* ent = IDToEntityMap[entityId];
			HEAVY_ASSERTE(ent, "Invalid entity ID");
			HEAVY_ASSERTE(ent->HasComponent(EnginePtr->GetComponentID<T>()), "Failed at RemoveComponent() - a component of a given UniqueID does not exist!");
			ent->ComponentPosessionFlags.set(EnginePtr->GetComponentID<T>(), false);
			T* component = static_cast<T*>(ent->Components[EnginePtr->GetComponentID<T>()]);
			ent->Components[EnginePtr->GetComponentID<T>()] = nullptr;
			component->~T();
			GetComponentAllocator<T>()->Free(component);
		}

		//------------------------------------------------------------------------------
		template<typename T>
		IterablePoolAllocator<T>* GetComponentAllocator()
		{
			size_t componentID = EnginePtr->GetComponentID<T>();
			HEAVY_ASSERTE(componentID < MAX_COMPONENTS_COUNT, "Invalid component ID");
			if (ComponentAllocators[componentID] == nullptr)
				ComponentAllocators[componentID] = new IterablePoolAllocator<T>(MAX_ENTITY_COUNT);
			return static_cast<IterablePoolAllocator<T>*>(ComponentAllocators[componentID]);
		}

		template<typename T, typename... Args>
		void AddWorldComponent(Args&&... args)
		{
			HEAVY_ASSERTE(!HasWorldComponent(EnginePtr->GetWorldComponentID<T>()), "Failed at AddWorldComponent() - a world component of a given type already exists!");
			Components[EnginePtr->GetWorldComponentID<T>()] = new T(std::forward<Args>(args)...);
			Components[EnginePtr->GetWorldComponentID<T>()]->SetFlags(eComponentBaseFlags::WORLD_COMPONENT);
		}

		//------------------------------------------------------------------------------
		template<typename T>
		void RemoveWorldComponent()
		{
			HEAVY_ASSERTE(HasWorldComponent(EnginePtr->GetWorldComponentID<T>()), "Failed at RemoveWorldComponent() - a component of a given type does not exist!");
			T* component = reinterpret_cast<T*>(Components[EnginePtr->GetWorldComponentID<T>()]);
			Components[EnginePtr->GetWorldComponentID<T>()] = nullptr;
			component->~T();
		}

		std::unordered_map<UniqueID, Entity*> IDToEntityMap;

		void RemoveComponentById(Entity* ent, size_t id);

		// Allocators
		PoolAllocator<Entity> EntitiesAllocator;
		IterablePoolAllocatorBase* ComponentAllocators[MAX_COMPONENTS_COUNT];
		Engine* EnginePtr;

		ComponentBase* Components[MAX_COMPONENTS_COUNT];

		DeferredTaskQueue DeferredTasksQueue;
	};

	//defined here due to circular inclusion problem; FIXME: circular inclusion
	template<typename T>
	T* Entity::GetComponent()
	{
		if (HasComponent(EntityWorld->GetEngine()->GetComponentID<T>()))
			return static_cast<T*>(Components[EntityWorld->GetEngine()->GetComponentID<T>()]);
		else
			return nullptr;
	}

} //namespace Poly
