#include "EnginePCH.hpp"

#include "CameraSystem.hpp"

void Poly::CameraSystem::CameraUpdatePhase(World* world)
{
	ScreenSize screen = world->GetEngine()->GetRenderingContext()->GetScreenSize();
	for (auto& kv : world->GetViewportWorldComponent().GetViewports())
	{
		const AARect& rect = kv.second.GetRect();
		float aspect = (rect.GetSize().X * screen.Width) / (rect.GetSize().Y * screen.Height);

		CameraComponent* cameraCmp = kv.second.GetCamera();
		ASSERTE(cameraCmp, "Viewport without camera?");
		TransformComponent* transformCmp = cameraCmp->GetSibling<TransformComponent>();
		if (transformCmp)
		{
			// reinit perspective
			if (cameraCmp->NeedsInit || cameraCmp->Aspect != aspect)
			{
				cameraCmp->NeedsInit = false;
				cameraCmp->Aspect = aspect;
				if (cameraCmp->IsPerspective)
					cameraCmp->Projection.SetPerspective(cameraCmp->Fov, cameraCmp->Aspect, cameraCmp->Near, cameraCmp->Far);
				else
				{
					//TODO implement orthographic projection
				}
			}

			cameraCmp->ModelView = transformCmp->GetGlobalTransformationMatrix().GetInversed();
			cameraCmp->MVP = cameraCmp->Projection * cameraCmp->ModelView;
		}
		else
			gConsole.LogError("Entity has camera component but no transform component!");
	}
}
