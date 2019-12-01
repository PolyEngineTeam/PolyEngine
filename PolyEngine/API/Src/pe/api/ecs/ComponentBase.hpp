#pragma once

#include <pe/Defines.hpp>

#include <pe/api/ecs/ComponentIDGenerator.hpp>
#include <pe/api/ecs/ComponentIDGeneratorImpl.hpp>
#include <pe/api/ecs/EntityBoundingChannel.hpp>
#include <pe/core/math/AABox.hpp>

namespace pe::api::ecs {
	class Entity;

	enum class eComponentBaseFlags
	{
		NONE = 0x00,
		NEWLY_CREATED = 0x01,
		ABOUT_TO_BE_REMOVED = 0x02
	};

	/// <summary>Base type for every component type</summary>
	class API_DLLEXPORT ComponentBase : public ::Poly::RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::pe::api::ecs::ComponentBase, ::Poly::RTTIBase) 
		{ 
			RTTI_PROPERTY_AUTONAME(Owner, ::Poly::RTTI::ePropertyFlag::NONE);
		}
	friend class Entity;
	public:
		// @todo change to pure virtual
		virtual size_t GetComponentID() const { ASSERTE(false, "This shouldn't be called!"); return 0; };


		/// <summary>Getter for a component of a specified type that shares ::pe::core::UniqueID with this one.</summary>
		/// <returns>Pointer to a component of a specified type or a nullptr, if it does not exist.</returns>
		template<typename T>
		T* GetSibling() { return static_cast<T*>(GetSibling(::pe::api::ecs::GetComponentID<T>())); }
		ComponentBase* GetSibling(size_t componentID);

		/// <summary>Getter for a component of a specified type that shares ::pe::core::UniqueID with this one.</summary>
		/// <returns>Pointer to a component of a specified type or a nullptr, if it does not exist.</returns>
		template<typename T>
		const T* GetSibling() const { return static_cast<const T*>(GetSibling(::pe::api::ecs::GetComponentID<T>())); }
		const ComponentBase* GetSibling(size_t componentID) const;

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

		void SetFlags(const ::pe::core::utils::EnumFlags<eComponentBaseFlags>& rhs) { Flags |= rhs; }
		void ResetFlags(const ::pe::core::utils::EnumFlags<eComponentBaseFlags>& rhs) { Flags &= ~rhs; }
		const ::pe::core::utils::EnumFlags<eComponentBaseFlags>& GetFlags() { return Flags; }
		bool CheckFlags(const ::pe::core::utils::EnumFlags<eComponentBaseFlags>& rhs) const { return (Flags & rhs) == rhs; }

		virtual std::optional<::pe::core::math::AABox> GetBoundingBox(eEntityBoundingChannel channel) const { return {}; }

	private:
		Entity* Owner = nullptr;

		::pe::core::utils::EnumFlags<eComponentBaseFlags> Flags;
	};
}
