#pragma once

#include <pe/Defines.hpp>
#include <pe/core/memory/SafePtrRoot.hpp>
#include <pe/core/math/AABox.hpp>
#include <pe/core/utils/EnumUtils.hpp>
#include <pe/core/rtti/RTTI.hpp>
#include <pe/api/ecs/EntityTransform.hpp>
#include <pe/api/ecs/ComponentIDGenerator.hpp>
#include <pe/api/ecs/Deleters.hpp>
#include <pe/api/ecs/Factories.hpp>
#include <pe/api/ecs/SceneAllocator.hpp>
#include <pe/api/ecs/EntityBoundingChannel.hpp>

namespace pe::api::ecs
{
	class Scene;
	class ComponentBase;

	/// <summary>Class that represent entity inside core engine systems. Should not be used anywhere else.</summary>
	class API_DLLEXPORT Entity : public ::pe::core::memory::SafePtrRoot
	{
		RTTI_DECLARE_TYPE_DERIVED(::pe::api::ecs::Entity, ::pe::core::memory::SafePtrRoot)
		{
			RTTI_PROPERTY_AUTONAME(m_name, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(m_transform, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(m_scene, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(m_parent, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_FACTORY_AUTONAME(m_children, entityFactory, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_FACTORY_AUTONAME(m_components, componentFactory, ::Poly::RTTI::ePropertyFlag::NONE);
		}
	public:
		Entity() = delete;
		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
		virtual ~Entity();

		Entity* operator=(const Entity&) = delete;
		Entity* operator=(Entity&&) = delete;

		// ########## Component queries ##########
		template<class T >
		[[nodiscard]] bool hasComponent() const { return HasComponent(GetComponentID<T>()); }
		[[nodiscard]] bool hasComponent(size_t ID) const;

		/// <summary>Checks whether there are all of the specified components under this Entity's ID.</summary>
		/// <param name="IDs">IDs is a bit field of component IDs to be checked for, see macro BIT()</param> 
		/// <returns>True if has queried components, false otherwise.</summary>
		/// <seealso cref="Entity.GetComponent()"/>
		/// <seealso cref="Entity.HasComponent()"/>
		[[nodiscard]] bool hasComponents(unsigned long int IDs) const;

		/// <summary>Gets a pointer to a component of a given ID.</summary>
		/// <returns>A pointer to a component or nullptr if it does not exist.</returns>
		template<typename T>
		[[nodiscard]] T* getComponent() { return static_cast<T*>(GetComponent(GetComponentID<T>())); }
		[[nodiscard]] ComponentBase* getComponent(size_t ctypeID) { return m_components[ctypeID].get(); }

		/// <summary>Gets a pointer to a component of a given ID.</summary>
		/// <returns>A pointer to a component or nullptr if it does not exist.</returns>
		template<typename T>
		[[nodiscard]] const T* getComponent() const { return static_cast<const T*>(GetComponent(GetComponentID<T>())); }
		[[nodiscard]] const ComponentBase* getComponent(size_t ctypeID) const { return m_components[ctypeID].get(); }

		// ########## Component management ##########
		template<typename T, typename... Args>
		void addComponent(Args&&... args)
		{ 
			auto component = GetSceneAllocator().template NewComponent<T>(std::forward<Args>(args)...);
			addComponentImpl(std::move(component));
		}

		template<typename T>
		void removeComponent() { RemoveComponent(GetComponentID<T>()); }
		void removeComponent(size_t componentID);

		// ########## Hierarchy queries ##########
		/// Returns pointer to parent entity. Returns nullptr if (and only if) this is the root of the scene.
		/// @return Pointer to parent entity.
		const Entity* getParent() const { return m_parent; }
		Entity* getParent() { return m_parent; }

		/// Returns collection of children of this entity.
		/// @return Collection of children.
		const std::vector<EntityUniquePtr>& getChildren() const { return m_children; }

		bool isRoot() const { return m_parent == nullptr; }
		bool isChildOfRoot() const { return m_parent ? m_parent->isRoot() : false; }
		bool containsChildRecursive(Entity* child) const;

		const Scene* getScene() const { HEAVY_ASSERTE(GetUUID(), "Entity was not properly initialized");  return m_scene; }
		Scene* getScene() { HEAVY_ASSERTE(GetUUID(), "Entity was not properly initialized");  return m_scene; }

		// ########## Hierarchy management ##########
		/// Reparents this entity. Entity cannot be parented to his children, to himself or to nothing (with exception to scene root).
		/// @param Entity* Pointer to new parent
		void setParent(Entity* parent);

		/// Adds new child to this entity. Child cannot be parent of this entity, equal to this entity or nullptr.
		/// @param Entity* Pointer to new child
		inline void addChild(Entity* child) { ASSERTE(child, "Child cannot be null!"); child->setParent(this); }

		// ########## Properties queries & management ##########
		const ::pe::core::storage::String& getName() const { return m_name; }
		void setName(const ::pe::core::storage::String& name) { m_name = name; }

		/// Returns transformation data of this entity.
		/// @return Transformation data of this entity.
		EntityTransform& getTransform() { return m_transform; }
		const EntityTransform& getTransform() const { return m_transform; }
	
		const ::pe::core::math::AABox& getLocalBoundingBox(eEntityBoundingChannel channel) const;
		::pe::core::math::AABox getGlobalBoundingBox(eEntityBoundingChannel channel) const;

	private:
		Entity(Scene* scene, Entity* parent = nullptr);

		void releaseFromParent();
		void setBBoxDirty();

		void addComponentImpl(ComponentUniquePtr<ComponentBase>&& component);

		SceneAllocator& getSceneAllocator();

		Entity* m_parent = nullptr;
		std::vector<EntityUniquePtr> m_children;

		::pe::core::storage::String m_name;
		EntityTransform m_transform;
		Scene* m_scene = nullptr;

		mutable ::pe::core::utils::EnumArray<::pe::core::math::AABox, eEntityBoundingChannel> m_localBBox;
		mutable ::pe::core::utils::EnumArray<bool, eEntityBoundingChannel> m_bboxDirty;

		// @todo(muniu) Fix the components flags and remove this assertion.
		STATIC_ASSERTE(MAX_COMPONENTS_COUNT <= 64, 
			"ComponentPosessionFlags will work only for MAX_COMPONENTS_COUNT<=64");
		std::bitset<MAX_COMPONENTS_COUNT> m_componentPosessionFlags;
		
		std::vector<ComponentUniquePtr<ComponentBase>> m_components;

		bool m_destructorActive = false;

		friend class SceneAllocator;
		friend class Scene;
	};
} //namespace Poly
