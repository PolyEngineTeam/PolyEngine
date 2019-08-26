#pragma once

#include <pe/Defines.hpp>
#include <pe/core/rtti/RTTI.hpp>
#include <pe/core/math/Vector.hpp>
#include <pe/core/math/Quaternion.hpp>

namespace Poly 
{
	class Entity;

	class ENGINE_DLLEXPORT EntityTransform final : public RTTIBase
	{
		RTTI_DECLARE_TYPE_DERIVED(::Poly::EntityTransform, ::Poly::RTTIBase)
		{
			RTTI_PROPERTY_AUTONAME(Owner, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(LocalTranslation, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(LocalRotation, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(LocalScale, RTTI::ePropertyFlag::NONE);
		
			RTTI_PROPERTY_AUTONAME(GlobalTranslation, RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(GlobalRotation, RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(GlobalScale, RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(ParentFromModel, RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(WorldFromModel, RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(LocalDirty, RTTI::ePropertyFlag::DONT_SERIALIZE);
			RTTI_PROPERTY_AUTONAME(GlobalDirty, RTTI::ePropertyFlag::DONT_SERIALIZE);
		}
	public:
		EntityTransform(Entity* owner = nullptr) : Owner(owner) {};
		~EntityTransform();

		const ::pe::core::math::Vector& GetGlobalTranslation() const;
		const ::pe::core::math::Vector& GetLocalTranslation() const { return LocalTranslation; };
		void SetLocalTranslation(const ::pe::core::math::Vector& position);
		void SetGlobalTranslation(const ::pe::core::math::Vector& position);

		const ::pe::core::math::Quaternion& GetGlobalRotation() const;
		const ::pe::core::math::Quaternion& GetLocalRotation() const { return LocalRotation; };
		void SetLocalRotation(const ::pe::core::math::Quaternion& quaternion);
		void SetGlobalRotation(const ::pe::core::math::Quaternion& quaternion);

		const ::pe::core::math::Vector& GetGlobalScale() const;
		const ::pe::core::math::Vector& GetLocalScale() const { return LocalScale; };
		void SetLocalScale(const ::pe::core::math::Vector& scale);
		void SetLocalScale(float scale) { SetLocalScale(::pe::core::math::Vector(scale, scale, scale)); };
		void SetGlobalScale(const ::pe::core::math::Vector& scale);

		const ::pe::core::math::Matrix& GetParentFromModel() const;
		const ::pe::core::math::Matrix& GetWorldFromModel() const;
		void SetParentFromModel(const ::pe::core::math::Matrix& parentFromModel);

		::pe::core::math::Vector GetLocalForward() const { return GetLocalRotation() * -::pe::core::math::Vector::UNIT_Z; }
		::pe::core::math::Vector GetLocalRight() const { return GetLocalRotation() * ::pe::core::math::Vector::UNIT_X; }
		::pe::core::math::Vector GetLocalUp() const { return GetLocalRotation() * ::pe::core::math::Vector::UNIT_Y; }
		::pe::core::math::Vector GetGlobalForward() const { return GetGlobalRotation() * -::pe::core::math::Vector::UNIT_Z; }
		::pe::core::math::Vector GetGlobalRight() const { return GetGlobalRotation() * ::pe::core::math::Vector::UNIT_X; }
		::pe::core::math::Vector GetGlobalUp() const { return GetGlobalRotation() * ::pe::core::math::Vector::UNIT_Y; }
		
	private:
		void UpdateParentTransform();

		Entity* Owner = nullptr;
		::pe::core::math::Vector LocalTranslation;
		mutable ::pe::core::math::Vector GlobalTranslation;
		::pe::core::math::Quaternion LocalRotation;
		mutable ::pe::core::math::Quaternion GlobalRotation;
		::pe::core::math::Vector LocalScale = ::pe::core::math::Vector(1.f, 1.f, 1.f);
		mutable ::pe::core::math::Vector GlobalScale = ::pe::core::math::Vector(1.f, 1.f, 1.f);

		mutable ::pe::core::math::Matrix ParentFromModel;
		mutable ::pe::core::math::Matrix WorldFromModel;
		mutable bool LocalDirty = true;
		mutable bool GlobalDirty = true;

		bool UpdateLocalTransformationCache() const;
		void UpdateGlobalTransformationCache() const;
		void SetGlobalDirty() const;

		friend class Entity;
	};
}