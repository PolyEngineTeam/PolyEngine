#include <pe/api/ecs/Entity.hpp>
#include <pe/api/ecs/EntityTransform.hpp>
#include <pe/api/ecs/Scene.hpp>
#include <pe/api/ecs/ComponentBase.hpp>

RTTI_DEFINE_TYPE(::pe::api::ecs::Entity);

namespace pe::api::ecs {

Entity::Entity(Scene* scene, Entity* parent)
	: m_transform(this), m_scene(scene), m_componentPosessionFlags(0)
{
	m_components.reserve(MAX_COMPONENTS_COUNT);
	for(size_t i=0; i<MAX_COMPONENTS_COUNT; ++i)
 		m_components.emplace_back(nullptr, getSceneAllocator().getComponentDeleter());

	if (parent)
		setParent(parent);
}

void Entity::setBBoxDirty()
{
	for (eEntityBoundingChannel channel : core::utils::IterateEnum<eEntityBoundingChannel>())
		m_bboxDirty[channel] = true;
	if (m_parent)
		m_parent->setBBoxDirty();
}

void Entity::releaseFromParent()
{
	if (m_parent != nullptr && !m_parent->m_destructorActive)
	{
		for (auto& child : m_parent->m_children)
		{
			if (child.get() == this)
				child.release();
		}
		DISCARD std::remove_if(m_parent->m_children.begin(), m_parent->m_children.end(), [](const EntityUniquePtr& p) { return p.get() == nullptr; });
		m_parent = nullptr;
		m_transform.updateParentTransform();
	}
}

Entity::~Entity()
{
	m_destructorActive = true;
	releaseFromParent();
	m_children.clear();
	m_components.clear();
}

void Entity::setParent(Entity* parent)
{
	ASSERTE(parent, "New parent cannot be null");
	ASSERTE(parent != this, "Cannot parent myself!");
	HEAVY_ASSERTE(!containsChildRecursive(parent), "Detected parenting cycle!");

	releaseFromParent();

	m_parent = parent;
	m_parent->m_children.push_back(EntityUniquePtr(this, getSceneAllocator().getEntityDeleter()));
	m_parent->setBBoxDirty();

	m_transform.updateParentTransform();
}

SceneAllocator& Entity::getSceneAllocator() { return m_scene->getSceneAllocator(); }

bool Entity::containsChildRecursive(Entity* child) const
{
	if (std::find_if(m_children.begin(), m_children.end(), [child](const EntityUniquePtr& p) { return p.get() == child; }) != m_children.end())
		return true;

	for (const EntityUniquePtr& myChild : m_children)
		if (myChild->containsChildRecursive(child))
			return true;

	return false;
}

const core::math::AABox& Entity::getLocalBoundingBox(eEntityBoundingChannel channel) const
{
	if (m_bboxDirty[channel])
	{
		m_localBBox[channel].SetMin(::pe::core::math::Vector::ZERO);
		m_localBBox[channel].SetSize(::pe::core::math::Vector::ZERO);

		// Update bounding box by children boxes
		for (auto& child : m_children)
		{
			core::math::AABox childBox = child->getLocalBoundingBox(channel);
			m_localBBox[channel].Expand(childBox.GetTransformed(child->getTransform().getParentFromModel()));
		}

		// m_components that affect bounding box
		for (auto& component : m_components)
		{
			if (!component)
				continue;

			auto bboxOpt = component->getBoundingBox(channel);
			if (bboxOpt.has_value())
				m_localBBox[channel].Expand(bboxOpt.value());
		}
		m_bboxDirty[channel] = false;
	}
	return m_localBBox[channel];
}

core::math::AABox Entity::getGlobalBoundingBox(eEntityBoundingChannel channel) const
{
	return getLocalBoundingBox(channel).GetTransformed(getTransform().getWorldFromModel());
}

bool Entity::hasComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_COMPONENTS_COUNT.");
	return m_componentPosessionFlags[ID];
}

void Entity::addComponentImpl(ComponentUniquePtr<ComponentBase>&& component)
{
	size_t ctypeID = component->getComponentID();
	HEAVY_ASSERTE(!hasComponent(ctypeID), "Failed at AttachComponent() - a component of the given type already attached!");

	component->m_owner = this;
	m_componentPosessionFlags.set(ctypeID, true);
	m_components[ctypeID] = std::move(component);
	HEAVY_ASSERTE(hasComponent(ctypeID), "Failed at AttachComponent() - the component was not attached!");

	setBBoxDirty();
}

void Entity::removeComponent(size_t componentID)
{
	HEAVY_ASSERTE(hasComponent(componentID), "Failed at RemoveComponent() - a component of the given type is not attached!");
	m_componentPosessionFlags.set(componentID, false);
	ComponentUniquePtr<ComponentBase> component = std::move(m_components[componentID]);
	HEAVY_ASSERTE(!hasComponent(componentID), "Failed at RemoveComponent() - the component was not detached!");
	
	setBBoxDirty();
}

[[nodiscard]] bool Entity::hasComponents(unsigned long int IDs) const
{
	return (m_componentPosessionFlags.to_ullong() & IDs) == IDs;
}

}