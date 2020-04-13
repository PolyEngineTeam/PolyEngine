#include <pe/api/ecs/EntityTransform.hpp>
#include <pe/api/ecs/Entity.hpp>

using namespace ::pe::core::math;

RTTI_DEFINE_TYPE(::pe::api::ecs::EntityTransform);

namespace pe::api::ecs {

//-----------------------------------------------------------------------------
EntityTransform::~EntityTransform() {
}

//-----------------------------------------------------------------------------
void EntityTransform::updateParentTransform()
{
	if (m_owner->getParent() != nullptr)
	{
		EntityTransform& parentTransform = m_owner->getParent()->getTransform();
		
		parentTransform.updateGlobalTransformationCache();
		m_localTranslation = m_localTranslation - parentTransform.m_globalTranslation;
		m_localRotation = m_localRotation * parentTransform.m_globalRotation.GetConjugated();
		core::math::Vector parentGlobalScale = parentTransform.m_globalScale;
		m_localScale.X = m_localScale.X / parentGlobalScale.X;
		m_localScale.Y = m_localScale.Y / parentGlobalScale.Y;
		m_localScale.Z = m_localScale.Z / parentGlobalScale.Z;
		updateLocalTransformationCache();
	}
	else
	{
		// Reset parenting
		core::math::Matrix globalTransform = getWorldFromModel();
		setParentFromModel(globalTransform);
	}
}

//------------------------------------------------------------------------------
const core::math::Vector& EntityTransform::getGlobalTranslation() const
{
	updateGlobalTransformationCache();
	return m_globalTranslation;
}

//------------------------------------------------------------------------------
void EntityTransform::setLocalTranslation(const core::math::Vector& position)
{
	m_localTranslation = position;
	m_localDirty = true;
	setGlobalDirty();
}

//------------------------------------------------------------------------------
void EntityTransform::setGlobalTranslation(const core::math::Vector& position)
{
	core::math::Vector currentGlobal = getGlobalTranslation();
	setLocalTranslation(getLocalTranslation() + (position - currentGlobal));
}

//------------------------------------------------------------------------------
const core::math::Quaternion& EntityTransform::getGlobalRotation() const
{
	updateGlobalTransformationCache();
	return m_globalRotation;
}

//------------------------------------------------------------------------------
void EntityTransform::setLocalRotation(const core::math::Quaternion& quaternion)
{
	m_localRotation = quaternion;
	m_localDirty = true;
	setGlobalDirty();
}

void EntityTransform::setGlobalRotation(const core::math::Quaternion& quaternion)
{
	core::math::Quaternion currentGlobal = getGlobalRotation();
	setLocalRotation(getLocalRotation() * (currentGlobal.Conjugate() * quaternion));
}

//------------------------------------------------------------------------------
const core::math::Vector& EntityTransform::getGlobalScale() const
{
	updateGlobalTransformationCache();
	return m_globalScale;
}

//------------------------------------------------------------------------------
void EntityTransform::setLocalScale(const core::math::Vector& scale)
{
	m_localScale = scale;
	m_localDirty = true;
	setGlobalDirty();
}

//------------------------------------------------------------------------------
void EntityTransform::setGlobalScale(const core::math::Vector& scale)
{
	core::math::Vector currentGlobal = getGlobalScale();
	core::math::Vector currLocal = getLocalScale();
	core::math::Vector mul;
	mul.X = currLocal.X * (scale.X / currentGlobal.X);
	mul.Y = currLocal.Y * (scale.Y / currentGlobal.Y);
	mul.Z = currLocal.Z * (scale.Z / currentGlobal.Z);
	setLocalScale(mul);
}

//------------------------------------------------------------------------------
const core::math::Matrix& EntityTransform::getParentFromModel() const
{
	updateLocalTransformationCache();
	return m_parentFromModel;
}

//------------------------------------------------------------------------------
const core::math::Matrix& EntityTransform::getWorldFromModel() const
{
	updateGlobalTransformationCache();
	return m_worldFromModel;
}

//------------------------------------------------------------------------------
void EntityTransform::setParentFromModel(const core::math::Matrix& parentFromModel)
{
	m_parentFromModel = parentFromModel;
	parentFromModel.Decompose(m_localTranslation, m_localRotation, m_localScale);
	m_localDirty = false;
	setGlobalDirty();
}

//------------------------------------------------------------------------------
bool EntityTransform::updateLocalTransformationCache() const
{
	if (m_localDirty)
	{
		m_parentFromModel = core::math::Matrix::Compose(m_localTranslation, m_localRotation, m_localScale);
		m_localDirty = false;
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
void EntityTransform::updateGlobalTransformationCache() const
{
	if (!m_globalDirty) return;

	const Entity* parent = m_owner->getParent();
	if (!parent)
	{
		m_worldFromModel = getParentFromModel();
	}
	else
	{
		core::math::Matrix worldFromParent = parent->getTransform().getWorldFromModel();
		m_worldFromModel = worldFromParent * getParentFromModel();
	}
	m_worldFromModel.Decompose(m_globalTranslation, m_globalRotation, m_globalScale);
	m_globalDirty = false;
}

//------------------------------------------------------------------------------
void EntityTransform::setGlobalDirty() const
{
	m_globalDirty = true;
	const auto& children = m_owner->getChildren();
	for (const EntityUniquePtr& c : children)
	{
		c->getTransform().setGlobalDirty();
	}
}

}