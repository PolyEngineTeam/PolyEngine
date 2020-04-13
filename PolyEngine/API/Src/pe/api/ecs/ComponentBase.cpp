#include <pe/api/APIPCH.hpp>

#include <pe/api/ecs/ComponentBase.hpp>
#include <pe/api/ecs/Entity.hpp>

RTTI_DEFINE_TYPE(::pe::api::ecs::ComponentBase);

namespace pe::api::ecs {

ComponentBase* ComponentBase::getSibling(size_t componentID)
{
	HEAVY_ASSERTE(m_owner, "Component was not properly initialized.");
	return m_owner->getComponent(componentID);
}

const ComponentBase* ComponentBase::getSibling(size_t componentID) const
{
	HEAVY_ASSERTE(m_owner, "Component was not properly initialized.");
	return m_owner->getComponent(componentID);
}

}