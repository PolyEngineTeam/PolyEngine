#include "EnginePCH.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
World::World()
	: EntitiesAllocator(MAX_ENTITY_COUNT)
{
	memset(ComponentAllocators, 0, sizeof(IterablePoolAllocatorBase*) * MAX_COMPONENTS_COUNT);
	memset(WorldComponents, 0, sizeof(ComponentBase*) * MAX_WORLD_COMPONENTS_COUNT);

	rootEntity = SpawnEntityInternal();
}

//------------------------------------------------------------------------------
World::~World()
{
	// copy entities
	if(rootEntity)
		DestroyEntity(rootEntity.Get());
	
	for (size_t i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	{
		if (ComponentAllocators[i])
			delete ComponentAllocators[i];
	}

	for (size_t i = 0; i < MAX_WORLD_COMPONENTS_COUNT; i++)
		if (WorldComponents[i])
			delete (WorldComponents[i]);
}

//------------------------------------------------------------------------------
Entity* World::SpawnEntity()
{
	Entity* ent = SpawnEntityInternal();
	ent->SetParent(rootEntity.Get());
	return ent;
}

//------------------------------------------------------------------------------
Entity * Poly::World::SpawnEntityInternal()
{
	Entity* ent = EntitiesAllocator.Alloc();
	::new(ent) Entity(this);
	return ent;
}

//------------------------------------------------------------------------------
void World::DestroyEntity(Entity* entity)
{
	HEAVY_ASSERTE(entity, "Invalid entity ID");

	auto childrenCopy = entity->GetChildren();
	for (Entity* child : childrenCopy)
		DestroyEntity(child);

	for (size_t i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	{
		if (entity->Components[i])
			RemoveComponentById(entity, i);
	}

	entity->~Entity();
	EntitiesAllocator.Free(entity);
}

//------------------------------------------------------------------------------
bool World::HasWorldComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_WORLD_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_WORLD_COMPONENTS_COUNT.");
	return WorldComponents[ID] != nullptr;
}

//------------------------------------------------------------------------------
void World::RemoveComponentById(Entity* ent, size_t id)
{
	HEAVY_ASSERTE(ent->Components[id], "Removing not present component");
	ent->Components[id]->~ComponentBase();
	ComponentAllocators[id]->Free(ent->Components[id]);
}
