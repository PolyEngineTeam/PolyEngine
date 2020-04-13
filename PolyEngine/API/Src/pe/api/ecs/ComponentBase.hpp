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
			RTTI_PROPERTY_AUTONAME(m_owner, ::Poly::RTTI::ePropertyFlag::NONE);
		}
	public:
		// @todo change to pure virtual
		virtual size_t getComponentID() const { ASSERTE(false, "This shouldn't be called!"); return 0; };


		/// <summary>Getter for a component of a specified type that shares ::pe::core::UniqueID with this one.</summary>
		/// <returns>Pointer to a component of a specified type or a nullptr, if it does not exist.</returns>
		template<typename T>
		T* getSibling() { return static_cast<T*>(getSibling(::pe::api::ecs::GetComponentID<T>())); }
		ComponentBase* getSibling(size_t componentID);

		/// <summary>Getter for a component of a specified type that shares ::pe::core::UniqueID with this one.</summary>
		/// <returns>Pointer to a component of a specified type or a nullptr, if it does not exist.</returns>
		template<typename T>
		const T* getSibling() const { return static_cast<const T*>(getSibling(::pe::api::ecs::GetComponentID<T>())); }
		const ComponentBase* getSibling(size_t componentID) const;

		inline const Entity* getOwner() const
		{
			HEAVY_ASSERTE(m_owner, "Component was not properly initialized.");
			return m_owner;
		}

		inline Entity* getOwner()
		{
			HEAVY_ASSERTE(m_owner, "Component was not properly initialized.");
			return m_owner;
		}

		void setFlags(const ::pe::core::utils::EnumFlags<eComponentBaseFlags>& rhs) { m_flags |= rhs; }
		void resetFlags(const ::pe::core::utils::EnumFlags<eComponentBaseFlags>& rhs) { m_flags &= ~rhs; }
		const ::pe::core::utils::EnumFlags<eComponentBaseFlags>& getFlags() { return m_flags; }
		bool checkFlags(const ::pe::core::utils::EnumFlags<eComponentBaseFlags>& rhs) const { return (m_flags & rhs) == rhs; }

		virtual std::optional<::pe::core::math::AABox> getBoundingBox(eEntityBoundingChannel channel) const { return {}; }

	private:
		friend class Entity;

		Entity* m_owner = nullptr;
		::pe::core::utils::EnumFlags<eComponentBaseFlags> m_flags;
	};
}
