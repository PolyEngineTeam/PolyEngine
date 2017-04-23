#include "CameraComponent.hpp"
#include "CameraSystem.hpp"
#include "World.hpp"
#include "TransformComponent.hpp"

void Poly::CameraSystem::CameraUpdatePhase(World* world)
{
	// TODO Temporary aspect, this should be replaced with propper viewport aspect
	const float aspect = 800.f / 600.f;
	auto allocator = world->GetComponentAllocator<BaseCameraComponent>();
	for (BaseCameraComponent& baseCameraCmp : *allocator)
	{
		TransformComponent* transformCmp = baseCameraCmp.GetSibling<TransformComponent>();
		if (transformCmp)
		{
			// reinit perspective
			if (baseCameraCmp.NeedsInit || baseCameraCmp.Aspect != aspect)
			{
				baseCameraCmp.NeedsInit = false;
				baseCameraCmp.Aspect = aspect;
				if (baseCameraCmp.IsPerspective)
					baseCameraCmp.Projection.SetPerspective(baseCameraCmp.Fov, baseCameraCmp.Aspect, baseCameraCmp.Near, baseCameraCmp.Far);
				else
				{
					//TODO implement orthographic projection
				}
			}

			baseCameraCmp.ModelView = transformCmp->GetGlobalTransformationMatrix().GetInversed();
			baseCameraCmp.MVP = baseCameraCmp.Projection * baseCameraCmp.ModelView;
		}
		else
			gConsole.LogError("Entity has camera component but no transform component!");
	}
}
