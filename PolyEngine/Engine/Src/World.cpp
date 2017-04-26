#include "EnginePCH.hpp"

#include "World.hpp"

using namespace Poly;

World::World(Poly::Engine* engine)
: EnginePtr(engine), EntitiesAllocator(MAX_ENTITY_COUNT)
{
	memset(ComponentAllocators, 0, sizeof(IterablePoolAllocatorBase*) * MAX_COMPONENTS_COUNT);
}

//------------------------------------------------------------------------------
Poly::World::~World()
{
	for (size_t i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	{
		if (ComponentAllocators[i])
			delete ComponentAllocators[i];
	}
}

//------------------------------------------------------------------------------
UniqueID World::SpawnEntity()
{
	Entity* ent = EntitiesAllocator.Alloc();
	::new(ent) Entity(this);
	IDToEntityMap[ent->EntityID] = ent;
	return ent->EntityID;
}

//------------------------------------------------------------------------------
void World::DestroyEntity(const UniqueID& entityId)
{
	Entity* ent = IDToEntityMap[entityId];
	HEAVY_ASSERTE(ent, "Invalid entity ID");
	for (size_t i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	{
		if (ent->Components[i])
			RemoveComponentById(ent, i);
	}
	IDToEntityMap.erase(entityId);
	ent->~Entity();
	EntitiesAllocator.Free(ent);
}

//------------------------------------------------------------------------------
void Poly::World::RemoveComponentById(Entity* ent, size_t id)
{
	HEAVY_ASSERTE(ent->Components[id], "Removing not present component");
	ent->Components[id]->~ComponentBase();
	ComponentAllocators[id]->Free(ent->Components[id]);
}
