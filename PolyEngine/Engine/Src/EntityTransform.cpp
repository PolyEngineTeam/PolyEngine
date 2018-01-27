#include "EnginePCH.hpp"

using namespace Poly;

//-----------------------------------------------------------------------------
EntityTransform::~EntityTransform() {
}

//-----------------------------------------------------------------------------
void EntityTransform::UpdateParentTransform()
{
	if (Owner->GetParent() != nullptr)
	{
		EntityTransform& parentTransform = Owner->GetParent()->GetTransform();
		
		parentTransform.UpdateGlobalTransformationCache();
		LocalTranslation = LocalTranslation - parentTransform.GlobalTranslation;
		LocalRotation = LocalRotation * parentTransform.GlobalRotation.GetConjugated();
		Vector parentGlobalScale = parentTransform.GlobalScale;
		LocalScale.X = LocalScale.X / parentGlobalScale.X;
		LocalScale.Y = LocalScale.Y / parentGlobalScale.Y;
		LocalScale.Z = LocalScale.Z / parentGlobalScale.Z;
		UpdateLocalTransformationCache();
	}
	else
	{
		// Reset parenting
		Matrix globalTransform = GetGlobalTransformationMatrix();
		SetLocalTransformationMatrix(globalTransform);
	}
}

//------------------------------------------------------------------------------
const Vector& EntityTransform::GetGlobalTranslation() const
{
	UpdateGlobalTransformationCache();
	return GlobalTranslation;
}

//------------------------------------------------------------------------------
void EntityTransform::SetLocalTranslation(const Vector& position)
{
	LocalTranslation = position;
	LocalDirty = true;
	SetGlobalDirty();
}

//------------------------------------------------------------------------------
const Quaternion& EntityTransform::GetGlobalRotation() const
{
	UpdateGlobalTransformationCache();
	return GlobalRotation;
}

//------------------------------------------------------------------------------
void EntityTransform::SetLocalRotation(const Quaternion& quaternion)
{
	LocalRotation = quaternion;
	LocalDirty = true;
	SetGlobalDirty();
}

//------------------------------------------------------------------------------
const Vector& EntityTransform::GetGlobalScale() const
{
	UpdateGlobalTransformationCache();
	return GlobalScale;
}

//------------------------------------------------------------------------------
void EntityTransform::SetLocalScale(const Vector& scale)
{
	LocalScale = scale;
	LocalDirty = true;
	SetGlobalDirty();
}

//------------------------------------------------------------------------------
const Matrix& EntityTransform::GetLocalTransformationMatrix() const
{
	UpdateLocalTransformationCache();
	return LocalTransform;
}

//------------------------------------------------------------------------------
const Matrix& EntityTransform::GetGlobalTransformationMatrix() const
{
	UpdateGlobalTransformationCache();
	return GlobalTransform;
}

//------------------------------------------------------------------------------
void EntityTransform::SetLocalTransformationMatrix(const Matrix& localTransformation)
{
	LocalTransform = localTransformation;
	localTransformation.Decompose(LocalTranslation, LocalRotation, LocalScale);
	LocalDirty = false;
	SetGlobalDirty();
}

//------------------------------------------------------------------------------
bool EntityTransform::UpdateLocalTransformationCache() const
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
void EntityTransform::UpdateGlobalTransformationCache() const
{
	if (!GlobalDirty) return;

	const Entity* parent = Owner->GetParent();
	if (!parent)
	{
		GlobalTransform = GetLocalTransformationMatrix();
	}
	else
	{
		GlobalTransform = parent->GetTransform().GetGlobalTransformationMatrix() * GetLocalTransformationMatrix();
	}
	GlobalTransform.Decompose(GlobalTranslation, GlobalRotation, GlobalScale);
	GlobalDirty = false;
}

//------------------------------------------------------------------------------
void EntityTransform::SetGlobalDirty() const
{
	GlobalDirty = true;
	const auto& children = Owner->GetChildren();
	for (Entity* c : children)
	{
		c->GetTransform().SetGlobalDirty();
	}
}
