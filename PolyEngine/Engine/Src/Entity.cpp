#include "EnginePCH.hpp"

#include "Entity.hpp"
#include "TransformComponent.hpp"

using namespace Poly;

void Poly::Entity::SetParent(Entity* parent)
{
	if (Parent)
	{
		TransformComponent* transform = GetComponent<TransformComponent>();
		if (!transform)
		{
			DeferredTaskSystem::AddComponentImmediate<TransformComponent>(EntityWorld, this);
			transform = GetComponent<TransformComponent>();
		}
		transform->ResetParentTransform();
		Parent->Children.Remove(this);
		Parent = nullptr;
	}

	ASSERTE(parent, "New parent cannot be null");
	TransformComponent* transform = GetComponent<TransformComponent>();
	if (!transform)
	{
		DeferredTaskSystem::AddComponentImmediate<TransformComponent>(EntityWorld, this);
		transform = GetComponent<TransformComponent>();
	}
		
	transform->SetParentTransform(parent->GetComponent<TransformComponent>());
	Parent = parent;
	Parent->Children.PushBack(this);
}

Entity::Entity(World * world)
: EntityID(UniqueID::Generate()), EntityWorld(world), ComponentPosessionFlags(0)
{
	memset(Components, 0, sizeof(ComponentBase*) * MAX_COMPONENTS_COUNT);
}

bool Entity::HasComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_COMPONENTS_COUNT.");
	return ComponentPosessionFlags[ID];
}
