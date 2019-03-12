#include <EnginePCH.hpp>

#include <ECS/EntityTransform.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;

RTTI_DEFINE_TYPE(::Poly::EntityTransform);

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
		Matrix globalTransform = GetWorldFromModel();
		SetParentFromModel(globalTransform);
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
void Poly::EntityTransform::SetGlobalTranslation(const Vector& position)
{
	Vector currentGlobal = GetGlobalTranslation();
	SetLocalTranslation(GetLocalTranslation() + (position - currentGlobal));
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

void Poly::EntityTransform::SetGlobalRotation(const Quaternion& quaternion)
{
	Quaternion currentGlobal = GetGlobalRotation();
	SetLocalRotation(GetLocalRotation() * (currentGlobal.Conjugate() * quaternion));
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
void Poly::EntityTransform::SetGlobalScale(const Vector& scale)
{
	Vector currentGlobal = GetGlobalScale();
	Vector currLocal = GetLocalScale();
	Vector mul;
	mul.X = currLocal.X * (scale.X / currentGlobal.X);
	mul.Y = currLocal.Y * (scale.Y / currentGlobal.Y);
	mul.Z = currLocal.Z * (scale.Z / currentGlobal.Z);
	SetLocalScale(mul);
}

//------------------------------------------------------------------------------
const Matrix& EntityTransform::GetParentFromModel() const
{
	UpdateLocalTransformationCache();
	return ParentFromModel;
}

//------------------------------------------------------------------------------
const Matrix& EntityTransform::GetWorldFromModel() const
{
	UpdateGlobalTransformationCache();
	return WorldFromModel;
}

//------------------------------------------------------------------------------
void EntityTransform::SetParentFromModel(const Matrix& parentFromModel)
{
	ParentFromModel = parentFromModel;
	parentFromModel.Decompose(LocalTranslation, LocalRotation, LocalScale);
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
		ParentFromModel = translation * rotation * scale;
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
		WorldFromModel = GetParentFromModel();
	}
	else
	{
		Matrix WorldFromParent = parent->GetTransform().GetWorldFromModel();
		WorldFromModel = WorldFromParent * GetParentFromModel();
	}
	WorldFromModel.Decompose(GlobalTranslation, GlobalRotation, GlobalScale);
	GlobalDirty = false;
}

//------------------------------------------------------------------------------
void EntityTransform::SetGlobalDirty() const
{
	GlobalDirty = true;
	const auto& children = Owner->GetChildren();
	for (const Entity::EntityUniquePtr& c : children)
	{
		c->GetTransform().SetGlobalDirty();
	}
}
