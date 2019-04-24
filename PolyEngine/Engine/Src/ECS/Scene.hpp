#pragma once

#include <Defines.hpp>
#include <ECS/Entity.hpp>
#include <ECS/ComponentBase.hpp>
#include <ECS/ComponentIterator.hpp>
#include <ECS/SceneComponentIteratorPolicy.hpp>
#include <Audio/SoundWorldComponent.hpp>
#include <Engine.hpp>


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
			//@todo add factory creation
			RTTI_PROPERTY_FACTORY_AUTONAME(RootEntity, &Entity::AllocateEntity, RTTI::ePropertyFlag::NONE);
		}
	public:
		using EntityUniquePtr = std::unique_ptr<Entity, EntityDeleter>;

		/// <summary>Allocates memory for entities, world components and components allocators.</summary>
		Scene();

		virtual ~Scene();

		Entity* GetRoot() const { return RootEntity.get(); }

		void BeforeDeserializationCallback() override;
		void AfterDeserializationCallback() override;

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

		template<typename T>
		bool HasWorldComponent() const
		{
			const auto ctypeID = GetWorldComponentID<T>();
			return HasWorldComponent(ctypeID);
		}

		/// <summary>Returns world component of given type.</summary>
		/// <returns>Pointer to world component</returns>
		template<typename T>
		T* GetWorldComponent()
		{
			return RootEntity->GetComponent<T>();
		}

		//------------------------------------------------------------------------------
		ComponentBase* GetWorldComponent(size_t ctypeID)
		{
			if (HasWorldComponent(ctypeID))
				return RootEntity->Components[ctypeID].get();
			return nullptr;
		}

		IterablePoolAllocatorBase* GetComponentAllocator(size_t componentID) 
		{
			HEAVY_ASSERTE(componentID < MAX_COMPONENTS_COUNT, "Invalid component ID");
			if (ComponentAllocators[componentID] == nullptr)
				ComponentAllocators[componentID] = ComponentManager::Get().CreateAllocator(componentID, MAX_ENTITY_COUNT);
			HEAVY_ASSERTE(ComponentAllocators[componentID], "Component allocator not created!");
			return ComponentAllocators[componentID];
		}
		ComponentDeleter& GetComponentDeleter() { return ComponentDel; }

		IterablePoolAllocator<Entity>& GetEntityAllocator() { return EntitiesAllocator; }
		EntityDeleter& GetEntityDeleter() { return EntityDel; }


		//------------------------------------------------------------------------------
		/// <summary>Returns statically set component type ID from 'Scene' group.</summary>
		/// <tparam name="T">Type of requested component.</tparam>
		/// <returns>Associated ID.</returns>
		template<typename T> static size_t GetWorldComponentID() noexcept
		{
			return ComponentsIDGroup::GetComponentTypeID<T>();
		}

		template<typename... T> static Dynarray<size_t> GetWorldComponentIDs() noexcept
		{
			return Dynarray<size_t>(std::initializer_list<size_t>{ GetWorldComponentID<T>()... } );
		}

		/// Iterator proxy
		template<typename PrimaryComponent, typename... SecondaryComponents>
		struct IteratorProxy : BaseObject<>
		{
			IteratorProxy(Scene* s) : S(s) {}
			ComponentIterator<PrimaryComponent, SecondaryComponents...> Begin()
			{
				return ComponentIterator<PrimaryComponent, SecondaryComponents...>(S->MakeSceneComponentIteratorPolicy<PrimaryComponent, SecondaryComponents...>(false));
			}
			ComponentIterator<PrimaryComponent, SecondaryComponents...> End()
			{
				return ComponentIterator<PrimaryComponent, SecondaryComponents...>(S->MakeSceneComponentIteratorPolicy<PrimaryComponent, SecondaryComponents...>(true));
			}
			auto begin() { return Begin(); }
			auto end() { return End(); }
			Scene* const S;
		};

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

	
	protected:
		
		template<typename PrimaryComponent, typename... SecondaryComponents>
		std::unique_ptr<SceneComponentIteratorPolicy> MakeSceneComponentIteratorPolicy(bool isEnd) 
		{
			size_t primary = GetWorldComponentID<PrimaryComponent>();
			if(!isEnd)
				return std::make_unique<SceneComponentIteratorPolicy>(SceneComponentIteratorPolicy::ConstructBegin(GetRoot(), primary)); 
			return std::make_unique<SceneComponentIteratorPolicy>(SceneComponentIteratorPolicy::ConstructEnd(GetRoot(), primary)); 
		}

	private:
		friend class SpawnEntityDeferredTask;
		friend class DestroyEntityDeferredTask;
		friend struct EntityDeleter;
		friend struct ComponentDeleter;
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
			entity->Components[ctypeID].reset(ptr);
			ptr->Owner = entity;
			HEAVY_ASSERTE(entity->HasComponent(ctypeID), "Failed at AddComponent() - the component was not added!");
			entity->SetBBoxDirty();
		}

		//------------------------------------------------------------------------------
		template<typename T>
		void RemoveComponent(Entity* entity)
		{
			const auto ctypeID = GetComponentID<T>();
			HEAVY_ASSERTE(entity, "Invalid entity ID");
			HEAVY_ASSERTE(entity->HasComponent(ctypeID), "Failed at RemoveComponent() - a component of a given UniqueID does not exist!");
			entity->ComponentPosessionFlags.set(ctypeID, false);
			entity->Components[ctypeID].reset(nullptr);
			HEAVY_ASSERTE(!entity->HasComponent(ctypeID), "Failed at AddComponent() - the component was not removed!");
			entity->SetBBoxDirty();
		}

		//------------------------------------------------------------------------------
		template<typename T>
		IterablePoolAllocator<T>* GetComponentAllocator()
		{
			const auto ctypeID = GetComponentID<T>();
			return static_cast<IterablePoolAllocator<T>*>(GetComponentAllocator(ctypeID));
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

		// Allocators
		IterablePoolAllocator<Entity> EntitiesAllocator;
		IterablePoolAllocatorBase* ComponentAllocators[MAX_COMPONENTS_COUNT];

		ComponentDeleter ComponentDel;
		EntityDeleter EntityDel;

		Entity::EntityUniquePtr RootEntity;
	};
} //namespace Poly
