#pragma once

#include <pe/Defines.hpp>
#include <pe/api/ecs/Factories.hpp>
#include <pe/api/ecs/SceneAllocator.hpp>
#include <pe/api/ecs/SceneIterationContext.hpp>

namespace pe::api::ecs {
	class Entity;

	/// <summary>Scene represents world/scene/level in engine.
	/// It contains entities, its components and world components.</summary>
	class API_DLLEXPORT Scene : public ::Poly::RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::pe::api::ecs::Scene, ::Poly::RTTIBase)
		{
			RTTI_PROPERTY_FACTORY_AUTONAME(m_rootEntity, &EntityFactory, ::Poly::RTTI::ePropertyFlag::NONE);
		}
	public:
		Scene();
		virtual ~Scene();

		// ########## Entity management ##########
		[[nodiscard]] Entity* SpawnEntity(Entity* parent = nullptr);
		void DestroyEntity(Entity* entity);
		
		// ########## Scene components management ##########
		template<typename T, typename... Args>
		void AddComponent(Args&&... args)
		{ 
			auto component = m_sceneAllocator.template NewComponent<T>(std::forward<Args>(args)...);
			AddComponentImpl(std::move(component));
		}

		template<typename T>
		void RemoveComponent() { RemoveComponent(GetComponentID<T>()); }
		void RemoveComponent(size_t componentID);

		// ########## Scene components queries ##########
		template<typename T>
		[[nodiscard]] bool HasComponent() const { return HasComponent(GetComponentID<T>()); }
		[[nodiscard]] bool HasComponent(size_t componentID) const;

		template<typename T>
		[[nodiscard]] T* GetComponent() { return static_cast<T*>(GetComponent(GetComponentID<T>())); }
		[[nodiscard]] ComponentBase* GetComponent(size_t componentID);

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
		[[nodiscard]] SceneIterationContext<PrimaryComponent, SecondaryComponents...>
		IterateComponents() { return { &m_sceneAllocator }; }

		// ########## Getters ##########
		[[nodiscard]] Entity* GetRoot() const { return m_rootEntity.get(); }
		[[nodiscard]] SceneAllocator& GetSceneAllocator() { return m_sceneAllocator; }
	private:
		void AddComponentImpl(ComponentBase* c);
		
		friend class EntityDeleter;
		friend class ComponentDeleter;

		SceneAllocator m_sceneAllocator;
		EntityUniquePtr m_rootEntity;
	};
} //namespace Poly
