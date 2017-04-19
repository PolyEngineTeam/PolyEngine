#include "TransformComponent.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
void TransformComponent::SetLocalTranslation(const Vector& position) {
	LocalTranslation = position;
	Dirty = true;
}

//------------------------------------------------------------------------------
void TransformComponent::SetLocalRotation(const Quaternion& quaternion) { 
	LocalRotation = quaternion; 
	Dirty = true;
}

//------------------------------------------------------------------------------
void TransformComponent::SetLocalScale(const Vector& scale) {
	LocalScale = scale;
	Dirty = true;
}

//------------------------------------------------------------------------------
const Matrix& TransformComponent::GetLocalTransformationMatrix() const {
	if(Dirty) {
		Matrix translation;
		Matrix rotation = LocalRotation.ToRotationMatrix();
		Matrix scale;
		translation.SetTranslation(LocalTranslation);
		scale.SetScale(LocalScale);
		LocalTransform = translation * rotation * scale;
		Dirty = false;
	}
	return LocalTransform;
}