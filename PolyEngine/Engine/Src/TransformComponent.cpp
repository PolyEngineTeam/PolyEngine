#include "EnginePCH.hpp"

using namespace Poly;

//-----------------------------------------------------------------------------
TransformComponent::~TransformComponent() {
}

//-----------------------------------------------------------------------------
void TransformComponent::SetParentTransform(TransformComponent* parent)
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
	}
	else
	{
		ResetParentTransform();
	}
}

//-----------------------------------------------------------------------------
void TransformComponent::ResetParentTransform()
{
	Matrix globalTransform = GetGlobalTransformationMatrix();
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
	SetGlobalDirty();
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
	SetGlobalDirty();
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
	SetGlobalDirty();
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
	SetGlobalDirty();
}

//------------------------------------------------------------------------------
bool TransformComponent::UpdateLocalTransformationCache() const
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
void TransformComponent::UpdateGlobalTransformationCache() const
{
	if (!GlobalDirty) return;

	const Entity* parent = GetOwner()->GetParent();
	if (parent->IsRoot())
	{
		GlobalTransform = GetLocalTransformationMatrix();
	}
	else
	{
		GlobalTransform = parent->GetComponent<TransformComponent>()->GetGlobalTransformationMatrix() * GetLocalTransformationMatrix();
	}
	GlobalTransform.Decompose(GlobalTranslation, GlobalRotation, GlobalScale);
	GlobalDirty = false;
}

//------------------------------------------------------------------------------
void TransformComponent::SetGlobalDirty() const
{
	GlobalDirty = true;
	const auto& children = GetOwner()->GetChildren();
	for (Entity* c : children)
	{
		c->GetComponent<TransformComponent>()->SetGlobalDirty();
	}
}
