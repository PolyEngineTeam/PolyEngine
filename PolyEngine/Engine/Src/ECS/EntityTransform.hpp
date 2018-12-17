#pragma once

#include <Defines.hpp>
#include <RTTI/RTTI.hpp>
#include <Math/Vector.hpp>
#include <Math/Quaternion.hpp>

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

		const Vector& GetGlobalTranslation() const;
		const Vector& GetLocalTranslation() const { return LocalTranslation; };
		void SetLocalTranslation(const Vector& position);
		void SetGlobalTranslation(const Vector& position);

		const Quaternion& GetGlobalRotation() const;
		const Quaternion& GetLocalRotation() const { return LocalRotation; };
		void SetLocalRotation(const Quaternion& quaternion);
		void SetGlobalRotation(const Quaternion& quaternion);

		const Vector& GetGlobalScale() const;
		const Vector& GetLocalScale() const { return LocalScale; };
		void SetLocalScale(const Vector& scale);
		void SetLocalScale(float scale) { SetLocalScale(Vector(scale, scale, scale)); };
		void SetGlobalScale(const Vector& scale);

		const Matrix& GetParentFromModel() const;
		const Matrix& GetWorldFromModel() const;
		void SetParentFromModel(const Matrix& parentFromModel);

		Vector GetLocalForward() const { return GetLocalRotation() * -Vector::UNIT_Z; }
		Vector GetLocalRight() const { return GetLocalRotation() * Vector::UNIT_X; }
		Vector GetLocalUp() const { return GetLocalRotation() * Vector::UNIT_Y; }
		Vector GetGlobalForward() const { return GetGlobalRotation() * -Vector::UNIT_Z; }
		Vector GetGlobalRight() const { return GetGlobalRotation() * Vector::UNIT_X; }
		Vector GetGlobalUp() const { return GetGlobalRotation() * Vector::UNIT_Y; }
		
	private:
		void UpdateParentTransform();

		Entity* Owner = nullptr;
		Vector LocalTranslation;
		mutable Vector GlobalTranslation;
		Quaternion LocalRotation;
		mutable Quaternion GlobalRotation;
		Vector LocalScale = Vector(1.f, 1.f, 1.f);
		mutable Vector GlobalScale = Vector(1.f, 1.f, 1.f);

		mutable Matrix ParentFromModel;
		mutable Matrix WorldFromModel;
		mutable bool LocalDirty = true;
		mutable bool GlobalDirty = true;

		bool UpdateLocalTransformationCache() const;
		void UpdateGlobalTransformationCache() const;
		void SetGlobalDirty() const;

		friend class Entity;
	};
}