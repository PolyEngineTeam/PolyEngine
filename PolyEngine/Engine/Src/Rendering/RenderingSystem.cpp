#include "EnginePCH.hpp"

using namespace Poly;

void RenderingSystem::RenderingPhase(World* world)
{	
	IRenderingDevice* device = gEngine->GetRenderingDevice();
	device->RenderWorld(world);
}
