#pragma once

#include "ComponentBase.hpp"

namespace Poly {
	class TransformComponent : public ComponentBase
	{
	public:
		const Vector& GetLocalTranslation() const { return LocalTranslation; };
		void SetLocalTranslation(const Vector& position);

		const Quaternion& GetLocalRotation() const { return LocalRotation; };
		void SetLocalRotation(const Quaternion& quaternion);
		
		const Vector& GetLocalScale() const { return LocalScale; };
		void SetLocalScale(const Vector& scale);
		void SetLocalScale(float scale) { SetLocalScale(Vector(scale, scale, scale)); };

		const Matrix& GetLocalTransformationMatrix() const;
		
	private:
		TransformComponent* Parent = nullptr;

		Vector LocalTranslation;
		Quaternion LocalRotation;
		Vector LocalScale = Vector(1.f, 1.f, 1.f);
		mutable Matrix LocalTransform;

		mutable bool Dirty = false;
	};
}