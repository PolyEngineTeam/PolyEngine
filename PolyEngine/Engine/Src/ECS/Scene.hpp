#pragma once

#include <unordered_map>
#include <Core.hpp>

#include <Memory/SafePtr.hpp>
#include "ECS/Entity.hpp"
#include "Engine.hpp"

#include "ECS/ComponentBase.hpp"
#include <Collections/OrderedMap.hpp>

namespace Poly {

	namespace DeferredTaskSystem
	{
		ENGINE_DLLEXPORT Entity* SpawnEntityImmediate(Scene* w);
		ENGINE_DLLEXPORT void DestroyEntityImmediate(Scene* w, Entity* entity);
		template<typename T, typename ...Args> T* AddComponentImmediate(Scene* w, Entity* entity, Args && ...args);
		template<typename T, typename ...Args> T* AddWorldComponentImmediate(Scene* w, Args && ...args);
		template<typename T> void RemoveWorldComponentImmediate(Scene* w);
	}
	struct InputState;

	/// <summary>Entities per world limit.</summary>
	constexpr size_t MAX_ENTITY_COUNT = 65536;

	/// <summary>Scene represents world/scene/level in engine.
	/// It contains entities, its components and world components.</summary>
	class ENGINE_DLLEXPORT Scene : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::Poly::Scene, ::Poly::RTTIBase)
		{
			RTTI_PROPERTY_AUTONAME(RootEntity, RTTI::ePropertyFlag::NONE);
		}
	public:
		struct ENGINE_DLLEXPORT ComponentDeleter final : public BaseObjectLiteralType<>
		{
			ComponentDeleter(Scene* s) : SceneHandle(s) {}

			template <typename T>
			void operator()(T* c)
			{
				DeleteComponentImpl(c, GetComponentID<T>());
			}

			void DeleteComponentImpl(ComponentBase* c, size_t componentID);

			Scene* SceneHandle = nullptr;
		};

		struct ENGINE_DLLEXPORT EntityDeleter final : public BaseObjectLiteralType<>
		{
			EntityDeleter(Scene* s) : SceneHandle(s) {}

			void operator()(Entity*);

			Scene* SceneHandle = nullptr;
		};

		/// <summary>Allocates memory for entities, world components and components allocators.</summary>
		Scene();

		virtual ~Scene();

		/// <summary>Gets a component of a specified type from entity with given UniqueID.</summary>
		/// <param name="entityId">UniqueID of the entity.</param>
		/// <returns>Pointer to a specified component or a nullptr, if none was found.</returns>
		/// <see cref="Scene.AddComponent()">
		/// <see cref="Scene.RemoveComponent()">
		template<typename T>
		T* GetComponent(Entity* entity)
		{
			HEAVY_ASSERTE(entity, "Invalid entity - entity with that ID does not exist!");
			return entity->GetComponent<T>();
		}

		/// <summary>Checks whether world has component of given ID.</summary>
		/// <param name="ID">Registered component ID.</param>
		/// <returns>True when world has component of given ID, false otherwise</returns>
		bool HasWorldComponent(size_t ID) const;

		/// <summary>Returns world component of given type.</summary>
		/// <returns>Pointer to world component</returns>
		template<typename T>
		T* GetWorldComponent()
		{
			return RootEntity->GetComponent<T>();
		}

		//------------------------------------------------------------------------------
		/// <summary>Returns statically set component type ID.</summary>
		/// <tparam name="T">Type of requested component.</tparam>
		/// <returns>Associated ID.</returns>
		template<typename T> static size_t GetComponentID() noexcept
		{
			return ComponentsIDGroup::GetComponentTypeID<T>();
		}

		//------------------------------------------------------------------------------
		/// <summary>Returns statically set component type ID from 'Scene' group.</summary>
		/// <tparam name="T">Type of requested component.</tparam>
		/// <returns>Associated ID.</returns>
		template<typename T> static size_t GetWorldComponentID() noexcept
		{
			return ComponentsIDGroup::GetComponentTypeID<T>();
		}

		template<typename PrimaryComponent, typename... SecondaryComponents>
		struct IteratorProxy;

		/// <summary>Allows iteration over multiple component types.
		/// Iterator dereferences to a tuple of component pointers.</summary>
		/// <example>To get the component out of the tuple use std::get()
		/// e.g. <code>std::get{YourComponentType*}(components)</code>
		/// If you have a C++17-compliant compiler, you can use structured bindings
		/// e.g. <code>for(auto [a, b] : world->IterateComponents{ComponentA, ComponentB}())</code></example>
		/// <param name="PrimaryComponent">At least one component type must be specified</param>
		/// <param name="SecondaryComponents">Additional component types (warning: returned pointers might be null!)</param>
		/// <returns>A proxy object that can be used in a range-for loop.</returns>
		/// <see cref="Scene.ComponentIterator"/>
		template<typename PrimaryComponent, typename... SecondaryComponents>
		IteratorProxy<PrimaryComponent, SecondaryComponents...> IterateComponents()
		{
			return {this};
		}

		/// Component iterator.
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

			ComponentIterator& operator++() { Increment(); return *this; }
			ComponentIterator operator++(int) { ComponentIterator ret(primary_iter); Increment(); return ret; }
			ComponentIterator& operator--() { Decrement(); return *this; }
			ComponentIterator operator--(int) { ComponentIterator ret(primary_iter); Decrement(); return ret; }

		private:
			//------------------------------------------------------------------------------
			template<int zero = 0>
			bool HasComponents(const Entity* entity) const { return true; }

			//------------------------------------------------------------------------------
			template<typename Component, typename... Rest>
			bool HasComponents(const Entity* entity) const
			{
				return entity->template HasComponent<Component>() && HasComponents<Rest...>(entity);
			}

			//------------------------------------------------------------------------------
			void Increment()
			{
				do { ++primary_iter; } while (primary_iter != End
					&& !HasComponents<SecondaryComponents...>(&*primary_iter->GetOwner()));
			}

			//------------------------------------------------------------------------------
			void Decrement()
			{
				do { --primary_iter; } while (primary_iter != End
					&& !HasComponents<SecondaryComponents...>(&*primary_iter->GetOwner()));
			}


			explicit ComponentIterator(typename IterablePoolAllocator<PrimaryComponent>::Iterator parent, Scene* const w) : primary_iter(parent), 
				Begin(w->GetComponentAllocator<PrimaryComponent>()->Begin()),
				End(w->GetComponentAllocator<PrimaryComponent>()->End()) {}
			friend struct IteratorProxy<PrimaryComponent, SecondaryComponents...>;

			typename IterablePoolAllocator<PrimaryComponent>::Iterator primary_iter;
			typename IterablePoolAllocator<PrimaryComponent>::Iterator Begin;
			typename IterablePoolAllocator<PrimaryComponent>::Iterator End;
		};

		/// Iterator proxy
		template<typename PrimaryComponent, typename... SecondaryComponents>
		struct IteratorProxy : BaseObject<>
		{
			IteratorProxy(Scene* w) : W(w) {}
			Scene::ComponentIterator<PrimaryComponent, SecondaryComponents...> Begin()
			{
				return ComponentIterator<PrimaryComponent, SecondaryComponents...>(W->GetComponentAllocator<PrimaryComponent>()->Begin(), W);
			}
			Scene::ComponentIterator<PrimaryComponent, SecondaryComponents...> End()
			{
				return ComponentIterator<PrimaryComponent, SecondaryComponents...>(W->GetComponentAllocator<PrimaryComponent>()->End(), W);
			}
			auto begin() { return Begin(); }
			auto end() { return End(); }
			Scene* const W;
		};

	private:
		friend class SpawnEntityDeferredTask;
		friend class DestroyEntityDeferredTask;
		template<typename T,typename... Args> friend class AddComponentDeferredTask;
		template<typename T> friend class RemoveComponentDeferredTask;

		friend Entity* DeferredTaskSystem::SpawnEntityImmediate(Scene*);
		friend void DeferredTaskSystem::DestroyEntityImmediate(Scene* w, Entity* entity);
		template<typename T, typename ...Args> friend T* DeferredTaskSystem::AddComponentImmediate(Scene* w, Entity* entity, Args && ...args);
		template<typename T, typename ...Args> friend T* DeferredTaskSystem::AddWorldComponentImmediate(Scene* w, Args&&... args);
		template<typename T> friend void DeferredTaskSystem::RemoveWorldComponentImmediate(Scene* w);

		//------------------------------------------------------------------------------
		Entity* SpawnEntity();
		Entity* SpawnEntityInternal();

		//------------------------------------------------------------------------------
		void DestroyEntity(Entity* entityId);

		//------------------------------------------------------------------------------
		template<typename T, typename... Args>
		void AddComponent(Entity* entity, Args&&... args)
		{
			const auto ctypeID = GetComponentID<T>();
			T* ptr = GetComponentAllocator<T>()->Alloc();
			::new(ptr) T(std::forward<Args>(args)...);
			HEAVY_ASSERTE(entity, "Invalid entity ID");
			HEAVY_ASSERTE(!entity->HasComponent(ctypeID), "Failed at AddComponent() - a component of a given UniqueID already exists!");
			entity->ComponentPosessionFlags.set(ctypeID, true);
			//entity->Components[ctypeID].reset(ptr);
			ptr->Owner = entity;
			HEAVY_ASSERTE(entity->HasComponent(ctypeID), "Failed at AddComponent() - the component was not added!");
		}

		//------------------------------------------------------------------------------
		template<typename T>
		void RemoveComponent(Entity* entity)
		{
			const auto ctypeID = GetComponentID<T>();
			HEAVY_ASSERTE(entity, "Invalid entity ID");
			HEAVY_ASSERTE(entity->HasComponent(ctypeID), "Failed at RemoveComponent() - a component of a given UniqueID does not exist!");
			entity->ComponentPosessionFlags.set(ctypeID, false);
			//entity->Components[ctypeID].reset(nullptr);
			HEAVY_ASSERTE(!entity->HasComponent(ctypeID), "Failed at AddComponent() - the component was not removed!");
		}

		//------------------------------------------------------------------------------
		template<typename T>
		IterablePoolAllocator<T>* GetComponentAllocator()
		{
			const auto ctypeID = GetComponentID<T>();
			HEAVY_ASSERTE(ctypeID < MAX_COMPONENTS_COUNT, "Invalid component ID");
			if (ComponentAllocators[ctypeID] == nullptr)
				ComponentAllocators[ctypeID] = new IterablePoolAllocator<T>(MAX_ENTITY_COUNT);
			return static_cast<IterablePoolAllocator<T>*>(ComponentAllocators[ctypeID]);
		}

		//------------------------------------------------------------------------------
		template<typename T, typename... Args>
		void AddWorldComponent(Args&&... args)
		{
			AddComponent<T>(RootEntity.get(), std::forward<Args>(args)...);
		}

		//------------------------------------------------------------------------------
		template<typename T>
		void RemoveWorldComponent()
		{
			RemoveComponent<T>(RootEntity.get());
		}

		void RemoveComponentById(Entity* ent, size_t id);

		std::unique_ptr<Entity, EntityDeleter> RootEntity;

		// Allocators
		IterablePoolAllocator<Entity> EntitiesAllocator;
		IterablePoolAllocatorBase* ComponentAllocators[MAX_COMPONENTS_COUNT];

		ComponentDeleter ComponentDel;
		EntityDeleter EntityDel;
	};

	//defined here due to circular inclusion problem; FIXME: circular inclusion
	template<typename T>
	T* Entity::GetComponent()
	{
		const auto ctypeID = Scene::GetComponentID<T>();
		if (HasComponent(ctypeID))
			return static_cast<T*>(Components[ctypeID].get());
		else
			return nullptr;
	}

	template<typename T>
	const T* Entity::GetComponent() const
	{
		const auto ctypeID = Scene::GetComponentID<T>();
		if (HasComponent(ctypeID))
			return static_cast<T*>(Components[ctypeID].get());
		else
			return nullptr;
	}

	template<class T >
	bool Entity::HasComponent() const
	{
		return HasComponent(Scene::GetComponentID<T>());
	}

} //namespace Poly
