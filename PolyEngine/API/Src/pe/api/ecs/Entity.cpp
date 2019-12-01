#include <pe/api/ecs/Entity.hpp>
#include <pe/api/ecs/EntityTransform.hpp>
#include <pe/api/ecs/Scene.hpp>
#include <pe/api/ecs/ComponentBase.hpp>

RTTI_DEFINE_TYPE(::pe::api::ecs::Entity);

namespace pe::api::ecs {

Entity::Entity(Scene* scene, Entity* parent)
	: Transform(this), m_scene(scene), ComponentPosessionFlags(0)
{
	Components.reserve(MAX_COMPONENTS_COUNT);
	for(size_t i=0; i<MAX_COMPONENTS_COUNT; ++i)
 		Components.emplace_back(nullptr, GetSceneAllocator().GetComponentDeleter());

	if (parent)
		SetParent(parent);
}

void Entity::SetBBoxDirty()
{
	for (eEntityBoundingChannel channel : core::utils::IterateEnum<eEntityBoundingChannel>())
		BBoxDirty[channel] = true;
	if (Parent)
		Parent->SetBBoxDirty();
}

void Entity::ReleaseFromParent()
{
	if (Parent != nullptr && !Parent->m_destructorActive)
	{
		for (auto& child : Parent->Children)
		{
			if (child.get() == this)
				child.release();
		}
		DISCARD std::remove_if(Parent->Children.begin(), Parent->Children.end(), [](const EntityUniquePtr& p) { return p.get() == nullptr; });
		Parent = nullptr;
		Transform.UpdateParentTransform();
	}
}

Entity::~Entity()
{
	m_destructorActive = true;
	ReleaseFromParent();
	Children.clear();
	Components.clear();
}

void Entity::SetParent(Entity* parent)
{
	ASSERTE(parent, "New parent cannot be null");
	ASSERTE(parent != this, "Cannot parent myself!");
	HEAVY_ASSERTE(!ContainsChildRecursive(parent), "Detected parenting cycle!");

	ReleaseFromParent();

	Parent = parent;
	Parent->Children.push_back(EntityUniquePtr(this, GetSceneAllocator().GetEntityDeleter()));
	Parent->SetBBoxDirty();

	Transform.UpdateParentTransform();
}

SceneAllocator& Entity::GetSceneAllocator() { return m_scene->GetSceneAllocator(); }

bool Entity::ContainsChildRecursive(Entity* child) const
{
	if (std::find_if(Children.begin(), Children.end(), [child](const EntityUniquePtr& p) { return p.get() == child; }) != Children.end())
		return true;

	for (const EntityUniquePtr& myChild : Children)
		if (myChild->ContainsChildRecursive(child))
			return true;

	return false;
}

const core::math::AABox& Entity::GetLocalBoundingBox(eEntityBoundingChannel channel) const
{
	if (BBoxDirty[channel])
	{
		LocalBBox[channel].SetMin(::pe::core::math::Vector::ZERO);
		LocalBBox[channel].SetSize(::pe::core::math::Vector::ZERO);

		// Update bounding box by children boxes
		for (auto& child : Children)
		{
			core::math::AABox childBox = child->GetLocalBoundingBox(channel);
			LocalBBox[channel].Expand(childBox.GetTransformed(child->GetTransform().GetParentFromModel()));
		}

		// Components that affect bounding box
		for (auto& component : Components)
		{
			if (!component)
				continue;

			auto bboxOpt = component->GetBoundingBox(channel);
			if (bboxOpt.has_value())
				LocalBBox[channel].Expand(bboxOpt.value());
		}
		BBoxDirty[channel] = false;
	}
	return LocalBBox[channel];
}

core::math::AABox Entity::GetGlobalBoundingBox(eEntityBoundingChannel channel) const
{
	return GetLocalBoundingBox(channel).GetTransformed(GetTransform().GetWorldFromModel());
}

bool Entity::HasComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_COMPONENTS_COUNT.");
	return ComponentPosessionFlags[ID];
}

void Entity::AddComponentImpl(ComponentUniquePtr<ComponentBase>&& component)
{
	size_t ctypeID = component->GetComponentID();
	HEAVY_ASSERTE(!HasComponent(ctypeID), "Failed at AttachComponent() - a component of the given type already attached!");

	component->Owner = this;
	ComponentPosessionFlags.set(ctypeID, true);
	Components[ctypeID] = std::move(component);
	HEAVY_ASSERTE(HasComponent(ctypeID), "Failed at AttachComponent() - the component was not attached!");

	SetBBoxDirty();
}

void Entity::RemoveComponent(size_t componentID)
{
	HEAVY_ASSERTE(HasComponent(componentID), "Failed at RemoveComponent() - a component of the given type is not attached!");
	ComponentPosessionFlags.set(componentID, false);
	ComponentUniquePtr<ComponentBase> component = std::move(Components[componentID]);
	HEAVY_ASSERTE(!HasComponent(componentID), "Failed at RemoveComponent() - the component was not detached!");
	
	SetBBoxDirty();
}

[[nodiscard]] bool Entity::HasComponents(unsigned long int IDs) const
{
	return (ComponentPosessionFlags.to_ullong() & IDs) == IDs;
}

}