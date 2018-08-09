#include "EnginePCH.hpp"

#include "ECS/Entity.hpp"
#include "ECS/EntityTransform.hpp"

using namespace Poly;

RTTI_DEFINE_TYPE(::Poly::Entity);

Entity::Entity(Scene* world, Entity* parent)
	: Transform(this), EntityScene(world), ComponentPosessionFlags(0)
{
	memset(Components, 0, sizeof(ComponentBase*) * MAX_COMPONENTS_COUNT);

	if (parent)
		SetParent(parent);
}

void Poly::Entity::SetBBoxDirty()
{
	for (eEntityBoundingChannel channel : IterateEnum<eEntityBoundingChannel>())
		BBoxDirty[channel] = true;
	if (Parent)
		Parent->SetBBoxDirty();
}

Poly::Entity::~Entity()
{
	if (Parent != nullptr)
	{
		Parent->Children.Remove(this);
	}
}

void Poly::Entity::SetParent(Entity* parent)
{
	ASSERTE(parent, "New parent cannot be null");
	ASSERTE(parent != this, "Cannot parent myself!");
	HEAVY_ASSERTE(!ContainsChildRecursive(parent), "Detected parenting cycle!");

	if (Parent)
	{
		Parent->Children.Remove(this);
		Parent->SetBBoxDirty();
		Parent = nullptr;
		Transform.UpdateParentTransform();
	}

	Parent = parent;
	Parent->Children.PushBack(this);
	Parent->SetBBoxDirty();
	Transform.UpdateParentTransform();
}

bool Poly::Entity::ContainsChildRecursive(Entity* child) const
{
	if (Children.Contains(child))
		return true;

	for (Entity* myChild : Children)
		if (myChild->ContainsChildRecursive(child))
			return true;

	return false;
}

const AABox& Poly::Entity::GetLocalBoundingBox(eEntityBoundingChannel channel) const
{
	if (BBoxDirty[channel])
	{
		LocalBBox[channel].SetPosition(Vector::ZERO);
		LocalBBox[channel].SetSize(Vector::ZERO);

		// Update bounding box by children boxes
		for (Entity* child : Children)
		{
			AABox childBox = child->GetLocalBoundingBox(channel);
			LocalBBox[channel].Expand(childBox.GetTransformed(child->GetTransform().GetParentFromModel()));
		}

		// Components that affect bounding box
		for (ComponentBase* component : Components)
		{
			if (!component)
				continue;

			auto bboxOpt = component->GetBoundingBox(channel);
			if (bboxOpt.HasValue())
				LocalBBox[channel].Expand(bboxOpt.Value());
		}
		BBoxDirty[channel] = false;
	}
	return LocalBBox[channel];
}

AABox Poly::Entity::GetGlobalBoundingBox(eEntityBoundingChannel channel) const
{
	return GetLocalBoundingBox(channel).GetTransformed(GetTransform().GetWorldFromModel());
}

bool Entity::HasComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_COMPONENTS_COUNT.");
	return ComponentPosessionFlags[ID];
}
