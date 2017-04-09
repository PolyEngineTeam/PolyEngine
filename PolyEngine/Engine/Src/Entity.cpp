#include "Entity.hpp"

using namespace Poly;

Entity::Entity(const World * world)
: EntityID(GetUniqueEntityID()), EntityWorld(world)
{
	memset(Components, 0, sizeof(ComponentBase*) * MAX_COMPONENTS_COUNT);
}
