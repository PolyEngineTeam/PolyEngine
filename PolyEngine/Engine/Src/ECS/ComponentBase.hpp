#pragma once

#include <Defines.hpp>
#include <Utils/Optional.hpp>
#include <ECS/Entity.hpp>
#include <ECS/ComponentIDGenerator.hpp>
#include <ECS/ComponentIDGeneratorImpl.hpp>

namespace Poly {
	enum class eComponentBaseFlags
	{
		NONE = 0x00,
		NEWLY_CREATED = 0x01,
		ABOUT_TO_BE_REMOVED = 0x02
	};

	/// <summary>Base type for every component type</summary>
	class ENGINE_DLLEXPORT ComponentBase : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::Poly::ComponentBase, ::Poly::RTTIBase) 
		{ 
			RTTI_PROPERTY_AUTONAME(Owner, RTTI::ePropertyFlag::NONE);
		}
	friend class Scene;
	public:
		// @todo change to pure virtual
		virtual size_t GetComponentID() const { ASSERTE(false, "This shouldn't be called!"); return 0; };


		/// <summary>Getter for a component of a specified type that shares UniqueID with this one.</summary>
		/// <returns>Pointer to a component of a specified type or a nullptr, if it does not exist.</returns>
		template<typename T>
		T* GetSibling()
		{
			HEAVY_ASSERTE(Owner, "Component was not properly initialized.");
			return Owner->GetComponent<T>();
		}

		/// <summary>Getter for a component of a specified type that shares UniqueID with this one.</summary>
		/// <returns>Pointer to a component of a specified type or a nullptr, if it does not exist.</returns>
		template<typename T>
		const T* GetSibling() const
		{
			HEAVY_ASSERTE(Owner, "Component was not properly initialized.");
			return Owner->GetComponent<T>();
		}

		inline const Entity* GetOwner() const
		{
			HEAVY_ASSERTE(Owner, "Component was not properly initialized.");
			return Owner;
		}

		inline Entity* GetOwner()
		{
			HEAVY_ASSERTE(Owner, "Component was not properly initialized.");
			return Owner;
		}

		inline EntityTransform& GetTransform() { return Owner->GetTransform(); }
		inline const EntityTransform& GetTransform() const { return Owner->GetTransform(); }

		void SetFlags(const EnumFlags<eComponentBaseFlags>& rhs) { Flags |= rhs; }
		void ResetFlags(const EnumFlags<eComponentBaseFlags>& rhs) { Flags &= ~rhs; }
		const EnumFlags<eComponentBaseFlags>& GetFlags() { return Flags; }
		bool CheckFlags(const EnumFlags<eComponentBaseFlags>& rhs) const { return (Flags & rhs) == rhs; }

		virtual Optional<AABox> GetBoundingBox(eEntityBoundingChannel channel) const { return {}; }

	private:
		Entity* Owner = nullptr;

		EnumFlags<eComponentBaseFlags> Flags;
	};
}
