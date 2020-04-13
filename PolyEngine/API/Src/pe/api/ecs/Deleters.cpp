#include <pe/api/APIPCH.hpp>

#include <pe/api/ecs/Deleters.hpp>
#include <pe/api/ecs/SceneAllocator.hpp>

namespace pe::api::ecs {

void EntityDeleter::operator()(Entity* e)
{
	m_sceneAllocator->deleteEntity(e);
}

void ComponentDeleter::operator()(ComponentBase* c)
{
	m_sceneAllocator->deleteComponent(c);
}

}