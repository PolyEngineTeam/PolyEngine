#include "CameraComponent.hpp"
#include "CameraSystem.hpp"
#include "World.hpp"
#include "TransformComponent.hpp"

void Poly::CameraSystem::CameraUpdatePhase(World* world)
{
	auto allocator = world->GetComponentAllocator<BaseCameraComponent>();
	for (BaseCameraComponent& baseCameraCmp : *allocator)
	{
		TransformComponent* transformCmp = baseCameraCmp.GetSibling<TransformComponent>();
		if (transformCmp)
		{

		}
		else
			gConsole.LogError("Entity has camera component but no transform component!");
	}
}
