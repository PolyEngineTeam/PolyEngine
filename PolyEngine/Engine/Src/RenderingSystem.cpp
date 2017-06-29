#include "EnginePCH.hpp"

using namespace Poly;

void RenderingSystem::RenderingPhase(World* world)
{	
	IRenderingDevice* device = gEngine->GetRenderingContext();
	device->RenderWorld(world);
}
