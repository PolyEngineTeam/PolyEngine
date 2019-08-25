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

		const core::math::Vector& GetGlobalTranslation() const;
		const core::math::Vector& GetLocalTranslation() const { return LocalTranslation; };
		void SetLocalTranslation(const core::math::Vector& position);
		void SetGlobalTranslation(const core::math::Vector& position);

		const core::math::Quaternion& GetGlobalRotation() const;
		const core::math::Quaternion& GetLocalRotation() const { return LocalRotation; };
		void SetLocalRotation(const core::math::Quaternion& quaternion);
		void SetGlobalRotation(const core::math::Quaternion& quaternion);

		const core::math::Vector& GetGlobalScale() const;
		const core::math::Vector& GetLocalScale() const { return LocalScale; };
		void SetLocalScale(const core::math::Vector& scale);
		void SetLocalScale(float scale) { SetLocalScale(core::math::Vector(scale, scale, scale)); };
		void SetGlobalScale(const core::math::Vector& scale);

		const core::math::Matrix& GetParentFromModel() const;
		const core::math::Matrix& GetWorldFromModel() const;
		void SetParentFromModel(const core::math::Matrix& parentFromModel);

		core::math::Vector GetLocalForward() const { return GetLocalRotation() * -core::math::Vector::UNIT_Z; }
		core::math::Vector GetLocalRight() const { return GetLocalRotation() * core::math::Vector::UNIT_X; }
		core::math::Vector GetLocalUp() const { return GetLocalRotation() * core::math::Vector::UNIT_Y; }
		core::math::Vector GetGlobalForward() const { return GetGlobalRotation() * -core::math::Vector::UNIT_Z; }
		core::math::Vector GetGlobalRight() const { return GetGlobalRotation() * core::math::Vector::UNIT_X; }
		core::math::Vector GetGlobalUp() const { return GetGlobalRotation() * core::math::Vector::UNIT_Y; }
		
	private:
		void UpdateParentTransform();

		Entity* Owner = nullptr;
		core::math::Vector LocalTranslation;
		mutable core::math::Vector GlobalTranslation;
		core::math::Quaternion LocalRotation;
		mutable core::math::Quaternion GlobalRotation;
		core::math::Vector LocalScale = core::math::Vector(1.f, 1.f, 1.f);
		mutable core::math::Vector GlobalScale = core::math::Vector(1.f, 1.f, 1.f);

		mutable core::math::Matrix ParentFromModel;
		mutable core::math::Matrix WorldFromModel;
		mutable bool LocalDirty = true;
		mutable bool GlobalDirty = true;

		bool UpdateLocalTransformationCache() const;
		void UpdateGlobalTransformationCache() const;
		void SetGlobalDirty() const;

		friend class Entity;
	};
}