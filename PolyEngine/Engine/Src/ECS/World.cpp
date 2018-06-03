#include "EnginePCH.hpp"

using namespace Poly;

RTTI_DEFINE_TYPE(::Poly::Scene);

//------------------------------------------------------------------------------
Scene::Scene()
	: EntitiesAllocator(MAX_ENTITY_COUNT)
{
	memset(ComponentAllocators, 0, sizeof(IterablePoolAllocatorBase*) * MAX_COMPONENTS_COUNT);
	//memset(WorldComponents, 0, sizeof(ComponentBase*) * MAX_WORLD_COMPONENTS_COUNT);
	//WorldComponents.Resize(MAX_WORLD_COMPONENTS_COUNT);

	rootEntity.reset(SpawnEntityInternal());
}

//------------------------------------------------------------------------------
Scene::~Scene()
{
	// copy entities
	if (rootEntity)
	{
		DestroyEntity(rootEntity.release());
	}
	
	Entity t;
	
	for (size_t i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	{
		if (ComponentAllocators[i])
			delete ComponentAllocators[i];
	}
}

//------------------------------------------------------------------------------
Entity* Scene::SpawnEntity()
{
	Entity* ent = SpawnEntityInternal();
	ent->SetParent(rootEntity.get());
	return ent;
}

//------------------------------------------------------------------------------
Entity * Poly::Scene::SpawnEntityInternal()
{
	Entity* ent = EntitiesAllocator.Alloc();
	::new(ent) Entity(this);
	return ent;
}

//------------------------------------------------------------------------------
void Scene::DestroyEntity(Entity* entity)
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
bool Scene::HasWorldComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_WORLD_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_WORLD_COMPONENTS_COUNT.");
	return WorldComponents.Get(ID).HasValue();
}

//------------------------------------------------------------------------------
void Scene::RemoveComponentById(Entity* ent, size_t id)
{
	HEAVY_ASSERTE(ent->Components[id], "Removing not present component");
	ent->Components[id]->~ComponentBase();
	ComponentAllocators[id]->Free(ent->Components[id]);
}
