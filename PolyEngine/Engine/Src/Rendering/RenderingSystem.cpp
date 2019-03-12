#include <EnginePCH.hpp>

#include <Rendering/RenderingSystem.hpp>
#include <Rendering/IRenderingDevice.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

void RenderingSystem::RenderingPhase(Scene* world)
{	
	IRenderingDevice* device = gEngine->GetRenderingDevice();
	device->RenderWorld(world);
}
