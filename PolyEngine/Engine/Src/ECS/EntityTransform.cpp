#include <EnginePCH.hpp>

#include <ECS/EntityTransform.hpp>
#include <ECS/Entity.hpp>

using namespace Poly;
using namespace ::pe::core::math;

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
		core::math::Vector parentGlobalScale = parentTransform.GlobalScale;
		LocalScale.X = LocalScale.X / parentGlobalScale.X;
		LocalScale.Y = LocalScale.Y / parentGlobalScale.Y;
		LocalScale.Z = LocalScale.Z / parentGlobalScale.Z;
		UpdateLocalTransformationCache();
	}
	else
	{
		// Reset parenting
		core::math::Matrix globalTransform = GetWorldFromModel();
		SetParentFromModel(globalTransform);
	}
}

//------------------------------------------------------------------------------
const core::math::Vector& EntityTransform::GetGlobalTranslation() const
{
	UpdateGlobalTransformationCache();
	return GlobalTranslation;
}

//------------------------------------------------------------------------------
void EntityTransform::SetLocalTranslation(const core::math::Vector& position)
{
	LocalTranslation = position;
	LocalDirty = true;
	SetGlobalDirty();
}

//------------------------------------------------------------------------------
void Poly::EntityTransform::SetGlobalTranslation(const core::math::Vector& position)
{
	core::math::Vector currentGlobal = GetGlobalTranslation();
	SetLocalTranslation(GetLocalTranslation() + (position - currentGlobal));
}

//------------------------------------------------------------------------------
const core::math::Quaternion& EntityTransform::GetGlobalRotation() const
{
	UpdateGlobalTransformationCache();
	return GlobalRotation;
}

//------------------------------------------------------------------------------
void EntityTransform::SetLocalRotation(const core::math::Quaternion& quaternion)
{
	LocalRotation = quaternion;
	LocalDirty = true;
	SetGlobalDirty();
}

void Poly::EntityTransform::SetGlobalRotation(const core::math::Quaternion& quaternion)
{
	core::math::Quaternion currentGlobal = GetGlobalRotation();
	SetLocalRotation(GetLocalRotation() * (currentGlobal.Conjugate() * quaternion));
}

//------------------------------------------------------------------------------
const core::math::Vector& EntityTransform::GetGlobalScale() const
{
	UpdateGlobalTransformationCache();
	return GlobalScale;
}

//------------------------------------------------------------------------------
void EntityTransform::SetLocalScale(const core::math::Vector& scale)
{
	LocalScale = scale;
	LocalDirty = true;
	SetGlobalDirty();
}

//------------------------------------------------------------------------------
void Poly::EntityTransform::SetGlobalScale(const core::math::Vector& scale)
{
	core::math::Vector currentGlobal = GetGlobalScale();
	core::math::Vector currLocal = GetLocalScale();
	core::math::Vector mul;
	mul.X = currLocal.X * (scale.X / currentGlobal.X);
	mul.Y = currLocal.Y * (scale.Y / currentGlobal.Y);
	mul.Z = currLocal.Z * (scale.Z / currentGlobal.Z);
	SetLocalScale(mul);
}

//------------------------------------------------------------------------------
const core::math::Matrix& EntityTransform::GetParentFromModel() const
{
	UpdateLocalTransformationCache();
	return ParentFromModel;
}

//------------------------------------------------------------------------------
const core::math::Matrix& EntityTransform::GetWorldFromModel() const
{
	UpdateGlobalTransformationCache();
	return WorldFromModel;
}

//------------------------------------------------------------------------------
void EntityTransform::SetParentFromModel(const core::math::Matrix& parentFromModel)
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
		ParentFromModel = core::math::Matrix::Compose(LocalTranslation, LocalRotation, LocalScale);
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
		core::math::Matrix WorldFromParent = parent->GetTransform().GetWorldFromModel();
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
