#include "EnginePCH.hpp"

using namespace Poly;

RTTI_DEFINE_TYPE(::Poly::Scene);

//------------------------------------------------------------------------------
Scene::Scene()
	: EntitiesAllocator(MAX_ENTITY_COUNT), ComponentDel(this), EntityDel(this), RootEntity(nullptr, EntityDel)
{
	memset(ComponentAllocators, 0, sizeof(IterablePoolAllocatorBase*) * MAX_COMPONENTS_COUNT);

	RootEntity.reset(SpawnEntityInternal());
}

//------------------------------------------------------------------------------
Scene::~Scene()
{
	RootEntity.reset();
	
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
	ent->SetParent(RootEntity.get());
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

	/*auto childrenCopy = entity->GetChildren();
	for (Entity* child : childrenCopy)
		DestroyEntity(child);

	for (size_t i = 0; i < MAX_COMPONENTS_COUNT; ++i)
	{
		if (entity->Components[i])
			RemoveComponentById(entity, i);
	}

	entity->~Entity();
	EntitiesAllocator.Free(entity);*/
	EntityDel(entity);
}

//------------------------------------------------------------------------------
bool Scene::HasWorldComponent(size_t ID) const
{
	return RootEntity->HasComponent(ID);
}

//------------------------------------------------------------------------------
void Scene::RemoveComponentById(Entity* ent, size_t id)
{
	HEAVY_ASSERTE(ent->Components[id], "Removing not present component");
	ent->Components[id].reset();
}

//------------------------------------------------------------------------------
void Poly::Scene::EntityDeleter::operator()(Entity* e)
{
	e->~Entity();
	SceneHandle->EntitiesAllocator.Free(e);
}

//------------------------------------------------------------------------------
void Poly::Scene::ComponentDeleter::DeleteComponentImpl(ComponentBase* c, size_t componentID)
{
	c->~ComponentBase();
	SceneHandle->ComponentAllocators[componentID]->Free(c);
}
