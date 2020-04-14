#include <pe/api/APIPCH.hpp>

#include <pe/api/ecs/SceneAllocator.hpp>
#include <pe/api/ecs/Entity.hpp>
#include <pe/api/ecs/ComponentBase.hpp>

namespace pe::api::ecs {

SceneAllocator::SceneAllocator(Scene* scene)
	: m_scene(scene)
	, m_entitiesAllocator(new IterablePoolAllocator<Entity>(MAX_ENTITY_COUNT))
	, m_componentDeleter(this)
	, m_entityDeleter(this)
{
	ASSERTE(m_scene, "Scene cannot be nullptr!");
}

SceneAllocator::~SceneAllocator()
{
}

EntityUniquePtr SceneAllocator::newEntity()
{
	Entity* ent = (Entity*)m_entitiesAllocator->GenericAlloc();
	::new(ent) Entity(m_scene);
	return EntityUniquePtr(ent, m_entityDeleter);
}

void SceneAllocator::deleteEntity(Entity* e)
{
	e->~Entity();
	m_entitiesAllocator->GenericFree(e);
}

void SceneAllocator::deleteComponent(ComponentBase* c)
{
	const size_t componentID = c->getComponentID();
	c->~ComponentBase();
	getComponentAllocator(componentID).GenericFree(c);
}

SceneAllocator::IterablePoolAllocatorBase& 
SceneAllocator::getComponentAllocator(size_t componentID) 
{
	HEAVY_ASSERTE(componentID < MAX_COMPONENTS_COUNT, "Invalid component ID");
	if (!m_componentAllocators[componentID])
		m_componentAllocators[componentID] = ComponentManager::Get().CreateAllocator(componentID, MAX_ENTITY_COUNT);
	HEAVY_ASSERTE(m_componentAllocators[componentID], "Component allocator not created!");
	return *m_componentAllocators[componentID];
}

}