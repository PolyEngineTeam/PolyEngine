#include "EnginePCH.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
World::World()
	: EntitiesAllocator(MAX_ENTITY_COUNT)
{
	memset(ComponentAllocators, 0, sizeof(IterablePoolAllocatorBase*) * MAX_COMPONENTS_COUNT);
	memset(WorldComponents, 0, sizeof(ComponentBase*) * MAX_WORLD_COMPONENTS_COUNT);
}

//------------------------------------------------------------------------------
World::~World()
{
	// copy entities
	auto entityMap = IDToEntityMap;
	for (auto& kv : entityMap)
	{
		if(IDToEntityMap.find(kv.second->EntityID) != IDToEntityMap.end())
			DestroyEntity(kv.second->EntityID);
	}
	
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

	TransformComponent* transform = ent->GetComponent<TransformComponent>();
	if (transform)
	{
		for (auto it : transform->GetChildren())
			DestroyEntity(it->GetOwnerID());
	}

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
bool World::HasWorldComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_WORLD_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_WORLD_COMPONENTS_COUNT.");
	return static_cast<bool>(WorldComponents[ID]);
}

//------------------------------------------------------------------------------
void World::RemoveComponentById(Entity* ent, size_t id)
{
	HEAVY_ASSERTE(ent->Components[id], "Removing not present component");
	ent->Components[id]->~ComponentBase();
	ComponentAllocators[id]->Free(ent->Components[id]);
}
