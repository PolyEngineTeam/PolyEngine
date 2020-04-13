#pragma once

#include <pe/Defines.hpp>
#include <pe/core/rtti/RTTI.hpp>
#include <pe/core/math/Vector.hpp>
#include <pe/core/math/Quaternion.hpp>

namespace pe::api::ecs 
{
	class Entity;

	class API_DLLEXPORT EntityTransform final : public ::Poly::RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::pe::api::ecs::EntityTransform, ::Poly::RTTIBase)
		{
			RTTI_PROPERTY_AUTONAME(m_owner, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(m_localTranslation, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(m_localRotation, ::Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(m_localScale, ::Poly::RTTI::ePropertyFlag::NONE);
		
			RTTI_PROPERTY_AUTONAME(m_globalTranslation, ::Poly::RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(m_globalRotation, ::Poly::RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(m_globalScale, ::Poly::RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(m_parentFromModel, ::Poly::RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(m_worldFromModel, ::Poly::RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(m_localDirty, ::Poly::RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(m_globalDirty, ::Poly::RTTI::ePropertyFlag::DONT_SERIALIZE);
		}
	public:
		EntityTransform(Entity* owner = nullptr) : m_owner(owner) {};
		~EntityTransform();

		const ::pe::core::math::Vector& getGlobalTranslation() const;
		const ::pe::core::math::Vector& getLocalTranslation() const { return m_localTranslation; };
		void setLocalTranslation(const ::pe::core::math::Vector& position);
		void setGlobalTranslation(const ::pe::core::math::Vector& position);

		const ::pe::core::math::Quaternion& getGlobalRotation() const;
		const ::pe::core::math::Quaternion& getLocalRotation() const { return m_localRotation; };
		void setLocalRotation(const ::pe::core::math::Quaternion& quaternion);
		void setGlobalRotation(const ::pe::core::math::Quaternion& quaternion);

		const ::pe::core::math::Vector& getGlobalScale() const;
		const ::pe::core::math::Vector& getLocalScale() const { return m_localScale; };
		void setLocalScale(const ::pe::core::math::Vector& scale);
		void setLocalScale(float scale) { setLocalScale(::pe::core::math::Vector(scale, scale, scale)); };
		void setGlobalScale(const ::pe::core::math::Vector& scale);

		const ::pe::core::math::Matrix& getParentFromModel() const;
		const ::pe::core::math::Matrix& getWorldFromModel() const;
		void setParentFromModel(const ::pe::core::math::Matrix& parentFromModel);

		::pe::core::math::Vector getLocalForward() const { return getLocalRotation() * -::pe::core::math::Vector::UNIT_Z; }
		::pe::core::math::Vector getLocalRight() const { return getLocalRotation() * ::pe::core::math::Vector::UNIT_X; }
		::pe::core::math::Vector getLocalUp() const { return getLocalRotation() * ::pe::core::math::Vector::UNIT_Y; }
		::pe::core::math::Vector getGlobalForward() const { return getGlobalRotation() * -::pe::core::math::Vector::UNIT_Z; }
		::pe::core::math::Vector getGlobalRight() const { return getGlobalRotation() * ::pe::core::math::Vector::UNIT_X; }
		::pe::core::math::Vector getGlobalUp() const { return getGlobalRotation() * ::pe::core::math::Vector::UNIT_Y; }
		
	private:
		void updateParentTransform();

		Entity* m_owner = nullptr;
		::pe::core::math::Vector m_localTranslation;
		mutable ::pe::core::math::Vector m_globalTranslation;
		::pe::core::math::Quaternion m_localRotation;
		mutable ::pe::core::math::Quaternion m_globalRotation;
		::pe::core::math::Vector m_localScale = ::pe::core::math::Vector(1.f, 1.f, 1.f);
		mutable ::pe::core::math::Vector m_globalScale = ::pe::core::math::Vector(1.f, 1.f, 1.f);

		mutable ::pe::core::math::Matrix m_parentFromModel;
		mutable ::pe::core::math::Matrix m_worldFromModel;
		mutable bool m_localDirty = true;
		mutable bool m_globalDirty = true;

		bool updateLocalTransformationCache() const;
		void updateGlobalTransformationCache() const;
		void setGlobalDirty() const;

		friend class Entity;
	};
}