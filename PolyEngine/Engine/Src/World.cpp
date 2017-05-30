#include "EnginePCH.hpp"

using namespace Poly;

World::World(Engine* engine, bool base)
	: EntitiesAllocator(MAX_ENTITY_COUNT), EnginePtr(engine)
{
	memset(ComponentAllocators, 0, sizeof(IterablePoolAllocatorBase*) * MAX_COMPONENTS_COUNT);
	memset(WorldComponentAllocators, 0, sizeof(IterablePoolAllocatorBase*) * MAX_WORLD_COMPONENTS_COUNT);

	if (!base)
	{
		AddWorldComponent<InputWorldComponent>();
		AddWorldComponent<ViewportWorldComponent>();
		AddWorldComponent<TimeWorldComponent>();
	}
}


//------------------------------------------------------------------------------
World::~World()
{
	// copy entities
	auto entityMap = IDToEntityMap;
	for (auto& kv : entityMap)
	{
		DestroyEntity(kv.second->EntityID);
	}
	
	for (size_t i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	{
		if (ComponentAllocators[i])
			delete ComponentAllocators[i];
	}
	for (size_t i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	{
		if (WorldComponentAllocators[i])
			delete WorldComponentAllocators[i];
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

bool World::HasWorldComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_WORLD_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_COMPONENTS_COUNT.");
	return ComponentPosessionFlags[ID];
}

//------------------------------------------------------------------------------
void World::RemoveComponentById(Entity* ent, size_t id)
{
	HEAVY_ASSERTE(ent->Components[id], "Removing not present component");
	ent->Components[id]->~ComponentBase();
	ComponentAllocators[id]->Free(ent->Components[id]);
}
