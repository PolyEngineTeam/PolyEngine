#pragma once

#include "ComponentBase.hpp"

namespace Poly {
	class ENGINE_DLLEXPORT TransformComponent : public ComponentBase
	{
	public:
		const TransformComponent* GetParent() const { return Parent; }
		void SetParent(TransformComponent* parent);

		const Vector& GetLocalTranslation() const { return LocalTranslation; };
		void SetLocalTranslation(const Vector& position);

		const Quaternion& GetLocalRotation() const { return LocalRotation; };
		void SetLocalRotation(const Quaternion& quaternion);
		
		const Vector& GetLocalScale() const { return LocalScale; };
		void SetLocalScale(const Vector& scale);
		void SetLocalScale(float scale) { SetLocalScale(Vector(scale, scale, scale)); };

		const Matrix& GetLocalTransformationMatrix() const;
		const Matrix& GetGlobalTransformationMatrix() const;
		
	private:
		const TransformComponent* Parent = nullptr;

		Vector LocalTranslation;
		Quaternion LocalRotation;
		Vector LocalScale = Vector(1.f, 1.f, 1.f);
		mutable Matrix LocalTransform;
		mutable Matrix GlobalTransform;

		mutable bool LocalDirty = false;
		mutable bool GlobalDirty = false;

		const bool UpdateGlobalTransformationCache() const;
	};
}