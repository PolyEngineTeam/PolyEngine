#include "EnginePCH.hpp"

using namespace Poly;

void RenderingSystem::RenderingPhase(World* world)
{	
	IRenderingDevice* device = world->GetEngine()->GetRenderingContext();
	device->RenderWorld(world);
}
