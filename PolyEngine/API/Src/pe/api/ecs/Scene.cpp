#include <pe/api/APIPCH.hpp>

#include <pe/api/ecs/Scene.hpp>
#include <pe/api/ecs/Entity.hpp>

RTTI_DEFINE_TYPE(::pe::api::ecs::Scene);

namespace pe::api::ecs {

Scene::Scene()
	: m_sceneAllocator(this)
	, m_rootEntity(m_sceneAllocator.newEntity())
{
}

Scene::~Scene()
{
	m_rootEntity.reset();
}

Entity* Scene::spawnEntity(Entity* parent)
{
	EntityUniquePtr ent = m_sceneAllocator.newEntity();
	ent->setParent(parent ? parent : m_rootEntity.get());
	return ent.release();
}

void Scene::destroyEntity(Entity* entity)
{
	HEAVY_ASSERTE(entity, "Invalid entity ID");
	m_sceneAllocator.deleteEntity(entity);
}

[[nodiscard]] bool Scene::hasComponent(size_t componentID) const
{
	return m_rootEntity->hasComponent(componentID);
}

[[nodiscard]] ComponentBase* Scene::getComponent(size_t componentID)
{
	return m_rootEntity->getComponent(componentID);
}

void Scene::removeComponent(size_t componentID)
{
	m_rootEntity->removeComponent(componentID);
}

void Scene::addComponentImpl(ComponentUniquePtr<ComponentBase>&& component)
{
	m_rootEntity->addComponentImpl(std::move(component));
}

}