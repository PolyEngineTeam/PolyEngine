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
			RTTI_PROPERTY_AUTONAME(NameTemplate, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(Name, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(Transform, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(m_scene, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(Parent, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_FACTORY_AUTONAME(Children, EntityFactory, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_FACTORY_AUTONAME(Components, ComponentFactory, ::Poly::RTTI::ePropertyFlag::NONE);
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
		[[nodiscard]] bool HasComponent() const { return HasComponent(GetComponentID<T>()); }
		[[nodiscard]] bool HasComponent(size_t ID) const;

		/// <summary>Checks whether there are all of the specified components under this Entity's ID.</summary>
		/// <param name="IDs">IDs is a bit field of component IDs to be checked for, see macro BIT()</param> 
		/// <returns>True if has queried components, false otherwise.</summary>
		/// <seealso cref="Entity.GetComponent()"/>
		/// <seealso cref="Entity.HasComponent()"/>
		[[nodiscard]] bool HasComponents(unsigned long int IDs) const;

		/// <summary>Gets a pointer to a component of a given ID.</summary>
		/// <returns>A pointer to a component or nullptr if it does not exist.</returns>
		template<typename T>
		[[nodiscard]] T* GetComponent() { return static_cast<T*>(GetComponent(GetComponentID<T>())); }
		[[nodiscard]] ComponentBase* GetComponent(size_t ctypeID) { return Components[ctypeID].get(); }

		/// <summary>Gets a pointer to a component of a given ID.</summary>
		/// <returns>A pointer to a component or nullptr if it does not exist.</returns>
		template<typename T>
		[[nodiscard]] const T* GetComponent() const { return static_cast<const T*>(GetComponent(GetComponentID<T>())); }
		[[nodiscard]] const ComponentBase* GetComponent(size_t ctypeID) const { return Components[ctypeID].get(); }

		// ########## Component management ##########
		template<typename T, typename... Args>
		void AddComponent(Args&&... args)
		{ 
			auto component = GetSceneAllocator().template NewComponent<T>(std::forward<Args>(args)...);
			AddComponentImpl(std::move(component));
		}

		template<typename T>
		void RemoveComponent() { RemoveComponent(GetComponentID<T>()); }
		void RemoveComponent(size_t componentID);

		// ########## Hierarchy queries ##########
		/// Returns pointer to parent entity. Returns nullptr if (and only if) this is the root of the scene.
		/// @return Pointer to parent entity.
		const Entity* GetParent() const { return Parent; }
		Entity* GetParent() { return Parent; }

		/// Returns collection of children of this entity.
		/// @return Collection of children.
		const std::vector<EntityUniquePtr>& GetChildren() const { return Children; }

		bool IsRoot() const { return Parent == nullptr; }
		bool IsChildOfRoot() const { return Parent ? Parent->IsRoot() : false; }
		bool ContainsChildRecursive(Entity* child) const;

		const Scene* GetScene() const { HEAVY_ASSERTE(GetUUID(), "Entity was not properly initialized");  return m_scene; }
		Scene* GetScene() { HEAVY_ASSERTE(GetUUID(), "Entity was not properly initialized");  return m_scene; }

		// ########## Hierarchy management ##########
		/// Reparents this entity. Entity cannot be parented to his children, to himself or to nothing (with exception to scene root).
		/// @param Entity* Pointer to new parent
		void SetParent(Entity* parent);

		/// Adds new child to this entity. Child cannot be parent of this entity, equal to this entity or nullptr.
		/// @param Entity* Pointer to new child
		inline void AddChild(Entity* child) { ASSERTE(child, "Child cannot be null!"); child->SetParent(this); }

		// ########## Properties queries & management ##########
		const ::pe::core::storage::String& GetName() const { return Name; }
		void SetName(const ::pe::core::storage::String& name) { Name = name; }

		/// Returns transformation data of this entity.
		/// @return Transformation data of this entity.
		EntityTransform& GetTransform() { return Transform; }
		const EntityTransform& GetTransform() const { return Transform; }
	
		const ::pe::core::math::AABox& GetLocalBoundingBox(eEntityBoundingChannel channel) const;
		::pe::core::math::AABox GetGlobalBoundingBox(eEntityBoundingChannel channel) const;

	private:
		Entity(Scene* scene, Entity* parent = nullptr);

		void ReleaseFromParent();
		void SetBBoxDirty();

		void AddComponentImpl(ComponentUniquePtr<ComponentBase>&& component);

		SceneAllocator& GetSceneAllocator();

		Entity* Parent = nullptr;
		std::vector<EntityUniquePtr> Children;

		::pe::core::storage::String NameTemplate;
		::pe::core::storage::String Name;
		EntityTransform Transform;
		Scene* m_scene = nullptr;

		mutable ::pe::core::utils::EnumArray<::pe::core::math::AABox, eEntityBoundingChannel> LocalBBox;
		mutable ::pe::core::utils::EnumArray<bool, eEntityBoundingChannel> BBoxDirty;

		// @todo(muniu) Fix the components flags and remove this assertion.
		STATIC_ASSERTE(MAX_COMPONENTS_COUNT <= 64, 
			"ComponentPosessionFlags will work only for MAX_COMPONENTS_COUNT<=64");
		std::bitset<MAX_COMPONENTS_COUNT> ComponentPosessionFlags;
		
		std::vector<ComponentUniquePtr<ComponentBase>> Components;

		bool m_destructorActive = false;

		friend class SceneAllocator;
	};
} //namespace Poly
