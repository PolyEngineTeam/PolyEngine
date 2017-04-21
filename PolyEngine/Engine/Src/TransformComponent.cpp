#include "TransformComponent.hpp"

using namespace Poly;

//-----------------------------------------------------------------------------
TransformComponent::~TransformComponent() {
	if (Parent != nullptr)
	{
		//TODO should be replaced with something like
		//Parent->Children.Remove(this);
		for (int i = 0; i < Parent->Children.GetSize(); ++i)
		{
			if (Parent->Children[i] == this)
			{
				Parent->Children.Remove(i);
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
void TransformComponent::SetParent(TransformComponent* parent) 
{
	if (parent != nullptr)
	{
		Parent = parent;
		Parent->Children.PushBack(this);
		//TODO calculate new LocalTransformation based on parent GlobalTransformation
		LocalDirty = true;
		GlobalDirty = true;
	}
	else 
	{ 
		//remove parent?
		//recalculate GlobalTransform?
	}
}

//------------------------------------------------------------------------------
void TransformComponent::SetLocalTranslation(const Vector& position) 
{
	LocalTranslation = position;
	LocalDirty = true;
	GlobalDirty = true;
}

//------------------------------------------------------------------------------
void TransformComponent::SetLocalRotation(const Quaternion& quaternion) 
{ 
	LocalRotation = quaternion;
	LocalDirty = true;
	GlobalDirty = true;
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
	if (LocalDirty) 
	{
		Matrix translation;
		Matrix rotation = LocalRotation.ToRotationMatrix();
		Matrix scale;
		translation.SetTranslation(LocalTranslation);
		scale.SetScale(LocalScale);
		LocalTransform = translation * rotation * scale;
		LocalDirty = false;
	}
	return LocalTransform;
}

//------------------------------------------------------------------------------
const Matrix& TransformComponent::GetGlobalTransformationMatrix() const 
{
	UpdateGlobalTransformationCache();
	return GlobalTransform;
}

//------------------------------------------------------------------------------
const bool TransformComponent::UpdateGlobalTransformationCache() const 
{
	if (Parent == nullptr) 
	{
		if (GlobalDirty) {
			GlobalTransform = GetLocalTransformationMatrix();
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
			GlobalDirty = false;
			return true;
		} 
		else 
			return false;
	}
}