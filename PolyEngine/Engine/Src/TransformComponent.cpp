#include "TransformComponent.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
void TransformComponent::SetLocalTranslation(Vector position) {
	LocalTranslation = position;
	Dirty = true;
}

//------------------------------------------------------------------------------
void TransformComponent::SetLocalRotation(EulerAngles angles) { 
	LocalRotation = Quaternion(angles); 
}

//------------------------------------------------------------------------------
void TransformComponent::SetLocalScale(Vector scale) {
	LocalScale = scale;
	Dirty = true;
}

//------------------------------------------------------------------------------
Matrix TransformComponent::GetLocalTransformationMatrix() {
	Matrix rotation = LocalRotation.ToRotationMatrix();
	Matrix translation;
	Matrix scale;
	translation.SetTranslation(LocalTranslation);
	scale.SetScale(LocalScale);
	LocalTransform = rotation * translation * scale;
	Dirty = false;
	return LocalTransform;
}