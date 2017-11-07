#pragma once

#include "ComponentBase.hpp"

namespace Poly 
{
	class ENGINE_DLLEXPORT TransformComponent : public ComponentBase
	{
	public:
		TransformComponent(TransformComponent* parent = nullptr) { if(parent) SetParent(parent); };
		~TransformComponent();

		const TransformComponent* GetParent() const { return Parent; }
		void SetParent(TransformComponent* parent);
		void ResetParent();

		const Vector& GetGlobalTranslation() const;
		const Vector& GetLocalTranslation() const { return LocalTranslation; };
		void SetLocalTranslation(const Vector& position);

		const Quaternion& GetGlobalRotation() const;
		const Quaternion& GetLocalRotation() const { return LocalRotation; };
		void SetLocalRotation(const Quaternion& quaternion);

		const Vector& GetGlobalScale() const;
		const Vector& GetLocalScale() const { return LocalScale; };
		void SetLocalScale(const Vector& scale);
		void SetLocalScale(float scale) { SetLocalScale(Vector(scale, scale, scale)); };

		const Matrix& GetLocalTransformationMatrix() const;
		const Matrix& GetGlobalTransformationMatrix() const;
		void SetLocalTransformationMatrix(const Matrix& localTransformation);
		
		const Dynarray<TransformComponent*>& GetChildren() const { return Children; }
	private:
		TransformComponent* Parent = nullptr;
		Dynarray<TransformComponent*> Children;

		Vector LocalTranslation;
		mutable Vector GlobalTranslation;
		Quaternion LocalRotation;
		mutable Quaternion GlobalRotation;
		Vector LocalScale = Vector(1.f, 1.f, 1.f);
		mutable Vector GlobalScale = Vector(1.f, 1.f, 1.f);

		mutable Matrix LocalTransform;
		mutable Matrix GlobalTransform;
		mutable bool LocalDirty = false;
		mutable bool GlobalDirty = false;

		bool UpdateLocalTransformationCache() const;
		void UpdateGlobalTransformationCache() const;
		void SetGlobalDirty() const;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, TransformComponent)
}