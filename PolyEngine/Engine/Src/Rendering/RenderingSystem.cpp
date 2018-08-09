#include "EnginePCH.hpp"

using namespace Poly;

void RenderingSystem::RenderingPhase(Scene* world)
{	
	IRenderingDevice* device = gEngine->GetRenderingDevice();
	device->RenderWorld(world);
}
