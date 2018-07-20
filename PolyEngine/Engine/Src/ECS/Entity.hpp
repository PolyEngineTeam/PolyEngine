#pragma once

#include <Core.hpp>
#include <bitset>

#include <Memory/SafePtrRoot.hpp>
#include "ECS/EntityTransform.hpp"
#include "Engine.hpp"

namespace Poly
{
	class ComponentBase;
	constexpr unsigned int MAX_COMPONENTS_COUNT = 64;

	/// <summary>Class that represent entity inside core engine systems. Should not be used anywhere else.</summary>
	class ENGINE_DLLEXPORT Entity : public SafePtrRoot
	{
		RTTI_DECLARE_TYPE_DERIVED(::Poly::Entity, ::Poly::SafePtrRoot)
		{
			RTTI_PROPERTY_AUTONAME(NameTemplate, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(Name, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(Transform, RTTI::ePropertyFlag::NONE);
			//@todo(muniu) rttibase pointers serialization
			//RTTI_PROPERTY_AUTONAME(EntityScene, RTTI::ePropertyFlag::NONE);
			//RTTI_PROPERTY_AUTONAME(Parent, RTTI::ePropertyFlag::NONE);
			//RTTI_PROPERTY_AUTONAME(Children, RTTI::ePropertyFlag::NONE);
			//RTTI_PROPERTY_AUTONAME(Components, RTTI::ePropertyFlag::NONE);
		}
	public:
		Entity() = default;
		~Entity();

		const Scene* GetEntityScene() const { HEAVY_ASSERTE(GetUUID(), "Entity was not properly initialized");  return EntityScene; }
		Scene* GetEntityScene() { HEAVY_ASSERTE(GetUUID(), "Entity was not properly initialized");  return EntityScene; }

		/// <summary>Checks whether there is a component of a given ID under this Entity's ID.</summary>
		/// <param name="ID">ID of a component type</param>
		/// <returns>True if has queried component, false otherwise.</summary>
		/// <seealso cref="Entity.GetComponent()"/>
		/// <seealso cref="Entity.HasComponents()"/>
		bool HasComponent(size_t ID) const;

		/// <summary>Checks whether there is a component of a given type in this Entity.</summary>
		/// <param name="T">Component type.</param>
		/// <returns>True if has queried component, false otherwise.</summary>
		/// <seealso cref="Entity.GetComponent()"/>
		/// <seealso cref="Entity.HasComponents()"/>
		template<class T>
		bool HasComponent() const;

		/// <summary>Checks whether there are all of the specified components under this Entity's ID.</summary>
		/// <param name="IDs">IDs is a bit field of component IDs to be checked for, see macro BIT()</param> 
		/// <returns>True if has queried components, false otherwise.</summary>
		/// <seealso cref="Entity.GetComponent()"/>
		/// <seealso cref="Entity.HasComponent()"/>
		inline bool HasComponents(unsigned long int IDs) const
		{
			//TODO This bit field has to be reimplemented - it will not work for MAX_COMPONENTS_COUNT greater than 64 in its current state.
			STATIC_ASSERTE(MAX_COMPONENTS_COUNT <= 64, "MAX_COMPONENTS_COUNT greater than 64.");
			return (ComponentPosessionFlags.to_ullong() & IDs) == IDs;
		}

		/// <summary>Gets a pointer to a component of a given ID.</summary>
		/// <returns>A pointer to a component or nullptr if it does not exist.</returns>
		template<class T>
		T* GetComponent(); //defined in ECS/Scene.hpp due to circular inclusion problem; FIXME: circular inclusion

		/// <summary>Gets a pointer to a component of a given ID.</summary>
		/// <returns>A pointer to a component or nullptr if it does not exist.</returns>
		template<class T>
		const T* GetComponent() const; //defined in ECS/Scene.hpp due to circular inclusion problem; FIXME: circular inclusion

		/// We need these two in editor ehere we have to iterate through all components.
		ComponentBase* GetComponent(size_t ctypeID) { return Components[ctypeID]; }
		const ComponentBase* GetComponent(size_t ctypeID) const { return Components[ctypeID]; }

		/// Returns pointer to parent entity. Returns nullptr if (and only if) this is the root of the scene.
		/// @return Pointer to parent entity.
		const Entity* GetParent() const { return Parent; }
		Entity* GetParent() { return Parent; }

		/// Returns collection of children of this entity.
		/// @return Collection of children.
		const Dynarray<Entity*>& GetChildren() const { return Children; }

		/// Reparents this entity. Entity cannot be parented to his children, to himself or to nothing (with exception to scene root).
		/// @param Entity* Pointer to new parent
		void SetParent(Entity* parent);

		/// Adds new child to this entity. Child cannot be parent of this entity, equal to this entity or nullptr.
		/// @param Entity* Pointer to new child
		inline void AddChild(Entity* child) { ASSERTE(child, "Child cannot be null!"); child->SetParent(this); }

		const String& GetName() const { return Name; }
		void SetName(const String& name) { Name = name; }

		/// Returns transformation data of this entity.
		/// @return Transformation data of this entity.
		EntityTransform& GetTransform() { return Transform; }
		const EntityTransform& GetTransform() const { return Transform; }

		bool IsRoot() const { return Parent == nullptr; }
		bool IsChildOfRoot() const { return Parent ? Parent->IsRoot() : false; }
		bool ContainsChildRecursive(Entity* child) const;

	private:
		Entity(Scene* world, Entity* parent = nullptr);

		Entity* Parent = nullptr;
		Dynarray<Entity*> Children;

		String NameTemplate;
		String Name;
		EntityTransform Transform;
		Scene* EntityScene = nullptr;

		std::bitset<MAX_COMPONENTS_COUNT> ComponentPosessionFlags;
		ComponentBase* Components[MAX_COMPONENTS_COUNT];

		friend class Scene;
	};
} //namespace Poly
