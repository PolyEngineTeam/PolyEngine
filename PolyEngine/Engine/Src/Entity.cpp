#include "Entity.hpp"

using namespace Poly;

Entity::Entity(const World * world)
: EntityID(UniqueID::Generate()), EntityWorld(world)
{
	memset(Components, 0, sizeof(ComponentBase*) * MAX_COMPONENTS_COUNT);
}
