#pragma once

#include "ComponentBase.hpp"

namespace Poly {
	class TransformComponent : public ComponentBase
	{
	public:
		TransformComponent() {};

		Vector GetLocalTranslation() { return LocalTranslation; };
		void SetLocalTranslation(Vector position);
		void SetLocalTranslation(float x, float y, float z) { SetLocalTranslation(Vector(x, y, z)); };

		Quaternion GetLocalRotation() { return LocalRotation; };
		void SetLocalRotation(EulerAngles angles);
		
		Vector GetLocalScale() { return LocalScale; };
		void SetLocalScale(Vector scale);
		void SetLocalScale(float x, float y, float z) { SetLocalScale(Vector(x, y, z)); };

		Matrix GetLocalTransformationMatrix();
		
	private:
		TransformComponent* Parent = nullptr;

		Vector LocalTranslation;
		Quaternion LocalRotation;
		Vector LocalScale = Vector(1.f, 1.f, 1.f);
		Matrix LocalTransform;

		bool Dirty = false;
	};
}