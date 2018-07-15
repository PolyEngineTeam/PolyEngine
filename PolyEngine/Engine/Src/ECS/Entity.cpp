#include "EnginePCH.hpp"

#include "ECS/Entity.hpp"
#include "ECS/EntityTransform.hpp"

using namespace Poly;

RTTI_DEFINE_TYPE(::Poly::Entity);

Entity::Entity(Scene* world, Entity* parent)
	: Transform(this), EntityScene(world), ComponentPosessionFlags(0)
{
	//Components.Resize(MAX_COMPONENTS_COUNT);
	//std::fill(Components.Begin(), Components.End(), nullptr);

	if (parent)
		SetParent(parent);
}


Poly::Entity::~Entity()
{
	if (Parent != nullptr)
	{
		Parent->Children.Remove([this](const std::unique_ptr<Entity>& p) { return p.get() == this; });
	}
}

void Poly::Entity::SetParent(Entity* parent)
{
	ASSERTE(parent, "New parent cannot be null");
	ASSERTE(parent != this, "Cannot parent myself!");
	HEAVY_ASSERTE(!ContainsChildRecursive(parent), "Detected parenting cycle!");

	if (Parent)
	{
		Parent->Children.Remove([this](const std::unique_ptr<Entity>& p) { return p.get() == this; });
		Parent = nullptr;
		Transform.UpdateParentTransform();
	}

	Parent = parent;
	Parent->Children.PushBack(std::unique_ptr<Entity>(this));
	Transform.UpdateParentTransform();
}

bool Poly::Entity::ContainsChildRecursive(Entity* child) const
{
	if (Children.Contains([child](const std::unique_ptr<Entity>& p) { return p.get() == child; }))
		return true;

	for (const std::unique_ptr<Entity>& myChild : Children)
		if (myChild->ContainsChildRecursive(child))
			return true;

	return false;
}

bool Entity::HasComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_COMPONENTS_COUNT.");
	return ComponentPosessionFlags[ID];
}
