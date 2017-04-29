#include "EnginePCH.hpp"

#include "Entity.hpp"

using namespace Poly;

Entity::Entity(const World * world)
: EntityID(UniqueID::Generate()), EntityWorld(world), ComponentPosessionFlags(0)
{
	memset(Components, 0, sizeof(ComponentBase*) * MAX_COMPONENTS_COUNT);
}

bool Entity::HasComponent(size_t ID) const
{
	HEAVY_ASSERTE(ID < MAX_COMPONENTS_COUNT, "Invalid component ID - greater than MAX_COMPONENTS_COUNT.");
	return ComponentPosessionFlags[ID];
}
