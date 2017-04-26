#include "EnginePCH.hpp"

#include "TransformComponent.hpp"

using namespace Poly;

//-----------------------------------------------------------------------------
TransformComponent::~TransformComponent() {
	if (Parent != nullptr)
	{
		Parent->Children.Remove(this);
	}
}

//-----------------------------------------------------------------------------
void TransformComponent::SetParent(TransformComponent* parent) 
{
	if (parent != nullptr)
	{
		parent->UpdateGlobalTransformationCache();
		LocalTranslation = LocalTranslation - parent->GlobalTranslation;
		LocalRotation = LocalRotation * parent->GlobalRotation.GetConjugated();
		Vector parentGlobalScale = parent->GlobalScale;
		LocalScale.X = LocalScale.X / parentGlobalScale.X;
		LocalScale.Y = LocalScale.Y / parentGlobalScale.Y;
		LocalScale.Z = LocalScale.Z / parentGlobalScale.Z;
		UpdateLocalTransformationCache();

		Parent = parent;
		Parent->Children.PushBack(this);
	}
	else 
	{ 
		ResetParent();
	}
}

//-----------------------------------------------------------------------------
void TransformComponent::ResetParent()
{
	ASSERTE(Parent, "ResetParent() called with parent == nullptr");
	Matrix globalTransform = GetGlobalTransformationMatrix();
	Parent->Children.Remove(this);
	Parent = nullptr;
	SetLocalTransformationMatrix(globalTransform);
}

//------------------------------------------------------------------------------
const Vector& TransformComponent::GetGlobalTranslation() const
{
	UpdateGlobalTransformationCache();
	return GlobalTranslation;
}

//------------------------------------------------------------------------------
void TransformComponent::SetLocalTranslation(const Vector& position) 
{
	LocalTranslation = position;
	LocalDirty = true;
	GlobalDirty = true;
}

//------------------------------------------------------------------------------
const Quaternion& TransformComponent::GetGlobalRotation() const
{
	UpdateGlobalTransformationCache();
	return GlobalRotation;
}

//------------------------------------------------------------------------------
void TransformComponent::SetLocalRotation(const Quaternion& quaternion) 
{ 
	LocalRotation = quaternion;
	LocalDirty = true;
	GlobalDirty = true;
}

//------------------------------------------------------------------------------
const Vector& TransformComponent::GetGlobalScale() const
{
	UpdateGlobalTransformationCache();
	return GlobalScale;
}

//------------------------------------------------------------------------------
void TransformComponent::SetLocalScale(const Vector& scale) 
{
	LocalScale = scale;
	LocalDirty = true;
	GlobalDirty = true;
}

//------------------------------------------------------------------------------
const Matrix& TransformComponent::GetLocalTransformationMatrix() const 
{
	UpdateLocalTransformationCache();
	return LocalTransform;
}

//------------------------------------------------------------------------------
const Matrix& TransformComponent::GetGlobalTransformationMatrix() const 
{
	UpdateGlobalTransformationCache();
	return GlobalTransform;
}

//------------------------------------------------------------------------------
void TransformComponent::SetLocalTransformationMatrix(const Matrix& localTransformation)
{
	LocalTransform = localTransformation;
	localTransformation.Decompose(LocalTranslation, LocalRotation, LocalScale);
	LocalDirty = false;
	GlobalDirty = true;
}

//------------------------------------------------------------------------------
const bool TransformComponent::UpdateLocalTransformationCache() const
{
	if (LocalDirty)
	{
		Matrix translation;
		Matrix rotation = LocalRotation.ToRotationMatrix();
		Matrix scale;
		translation.SetTranslation(LocalTranslation);
		scale.SetScale(LocalScale);
		LocalTransform = translation * rotation * scale;
		LocalDirty = false;
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
const bool TransformComponent::UpdateGlobalTransformationCache() const 
{
	if (Parent == nullptr) 
	{
		if (GlobalDirty) {
			GlobalTransform = GetLocalTransformationMatrix();
			GlobalTransform.Decompose(GlobalTranslation, GlobalRotation, GlobalScale);
			GlobalDirty = false;
			return true;
		} 
		else 
			return false;
	}
	else 
	{
		if (Parent->UpdateGlobalTransformationCache() || GlobalDirty) 
		{
			GlobalTransform = Parent->GlobalTransform * GetLocalTransformationMatrix();
			GlobalTransform.Decompose(GlobalTranslation, GlobalRotation, GlobalScale);
			GlobalDirty = false;
			return true;
		} 
		else 
			return false;
	}
}