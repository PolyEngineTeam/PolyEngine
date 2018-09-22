#include <EnginePCH.hpp>

#include <ECS/Scene.hpp>

using namespace Poly;

RTTI_DEFINE_TYPE(::Poly::Scene);

//------------------------------------------------------------------------------
Scene::Scene()
	: EntitiesAllocator(MAX_ENTITY_COUNT), ComponentDel(), EntityDel(), RootEntity(nullptr, EntityDel)
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
void Poly::Scene::BeforeDeserializationCallback()
{
	gEngine->SetCurrentlySerializedScene(this);
}

//------------------------------------------------------------------------------
void Poly::Scene::AfterDeserializationCallback()
{
	gEngine->SetCurrentlySerializedScene(nullptr);
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

	entity->ReleaseFromParent();
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