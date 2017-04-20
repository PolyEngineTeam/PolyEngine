#include "TransformComponent.hpp"

using namespace Poly;

//-----------------------------------------------------------------------------
void TransformComponent::SetParent(TransformComponent* parent) {
	Parent = parent;
	//TODO calculate new LocalTransformation based on parent GlobalTransformation
	Dirty = true;
}

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
	if (Dirty) {
		Matrix translation;
		Matrix rotation = LocalRotation.ToRotationMatrix();
		Matrix scale;
		translation.SetTranslation(LocalTranslation);
		scale.SetScale(LocalScale);
		LocalTransform = translation * rotation * scale;
		Dirty = false;
		UpdateGlobalTransformationCache();
	}
	return LocalTransform;
}

//------------------------------------------------------------------------------
const Matrix& TransformComponent::GetGlobalTransformationMatrix() const {
	UpdateGlobalTransformationCache();
	return GlobalTransform;
}

//------------------------------------------------------------------------------
const bool TransformComponent::UpdateGlobalTransformationCache() const {
	if (Parent == nullptr) {
		if (Dirty) {
			GlobalTransform = GetLocalTransformationMatrix();
			return true;
		} else {
			GlobalTransform = GetLocalTransformationMatrix();
			return false;
		}
	}
	else {
		if (Parent->UpdateGlobalTransformationCache() || Dirty) {
			GlobalTransform = Parent->GetGlobalTransformationMatrix() * GetLocalTransformationMatrix();
			return true;
		} else {
			return false;
		}
	}
}