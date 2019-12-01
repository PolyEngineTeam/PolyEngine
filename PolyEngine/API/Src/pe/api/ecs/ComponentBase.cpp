#include <pe/api/APIPCH.hpp>

#include <pe/api/ecs/ComponentBase.hpp>
#include <pe/api/ecs/Entity.hpp>

RTTI_DEFINE_TYPE(::pe::api::ecs::ComponentBase);

namespace pe::api::ecs {

ComponentBase* ComponentBase::GetSibling(size_t componentID)
{
	HEAVY_ASSERTE(Owner, "Component was not properly initialized.");
	return Owner->GetComponent(componentID);
}

const ComponentBase* ComponentBase::GetSibling(size_t componentID) const
{
	HEAVY_ASSERTE(Owner, "Component was not properly initialized.");
	return Owner->GetComponent(componentID);
}

}