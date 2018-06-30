#include "EnginePCH.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
World::World()
	: EntitiesAllocator(MAX_ENTITY_COUNT)
{
	memset(ComponentAllocators, 0, sizeof(IterablePoolAllocatorBase*) * MAX_COMPONENTS_COUNT);

	RootEntity = SpawnEntityInternal();
}

//------------------------------------------------------------------------------
World::~World()
{
	// copy entities
	if(RootEntity)
		DestroyEntity(RootEntity.Get());
	
	for (size_t i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	{
		if (ComponentAllocators[i])
			delete ComponentAllocators[i];
	}
}

//------------------------------------------------------------------------------
Entity* World::SpawnEntity()
{
	Entity* ent = SpawnEntityInternal();
	return ent;
}

//------------------------------------------------------------------------------
Entity * Poly::World::SpawnEntityInternal()
{
	Entity* ent = EntitiesAllocator.Alloc();
	::new(ent) Entity(this, RootEntity.Get());
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
	return RootEntity->HasComponent(ID);
}

//------------------------------------------------------------------------------
void World::RemoveComponentById(Entity* ent, size_t id)
{
	HEAVY_ASSERTE(ent->Components[id], "Removing not present component");
	ent->Components[id]->~ComponentBase();
	ComponentAllocators[id]->Free(ent->Components[id]);
}
