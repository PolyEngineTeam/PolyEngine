#include <pe/api/APIPCH.hpp>

#include <pe/api/ecs/Scene.hpp>
#include <pe/api/ecs/Entity.hpp>

RTTI_DEFINE_TYPE(::pe::api::ecs::Scene);

namespace pe::api::ecs {

Scene::Scene()
	: m_sceneAllocator(this)
	, m_rootEntity(m_sceneAllocator.NewEntity())
{
}

Scene::~Scene()
{
	m_rootEntity.reset();
}

Entity* Scene::SpawnEntity(Entity* parent)
{
	EntityUniquePtr ent = m_sceneAllocator.NewEntity();
	ent->SetParent(parent ? parent : m_rootEntity.get());
	return ent.release();
}

void Scene::DestroyEntity(Entity* entity)
{
	HEAVY_ASSERTE(entity, "Invalid entity ID");
	m_sceneAllocator.DeleteEntity(entity);
}

[[nodiscard]] bool Scene::HasComponent(size_t componentID) const
{
	return m_rootEntity->HasComponent(componentID);
}

[[nodiscard]] ComponentBase* Scene::GetComponent(size_t componentID)
{
	return m_rootEntity->GetComponent(componentID);
}

void Scene::RemoveComponent(size_t componentID)
{
	return m_rootEntity->RemoveComponent(componentID);
}

}