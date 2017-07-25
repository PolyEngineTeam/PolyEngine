#include "EnginePCH.hpp"

#include "CameraSystem.hpp"

void Poly::CameraSystem::CameraUpdatePhase(World* world)
{
	ScreenSize screen = gEngine->GetRenderingDevice()->GetScreenSize();
	for (auto& kv : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		const AABox& rect = kv.second.GetRect();
		float aspect = (rect.GetSize().X * screen.Width) / (rect.GetSize().Y * screen.Height);

		CameraComponent* cameraCmp = kv.second.GetCamera();
		ASSERTE(cameraCmp, "Viewport without camera?");
		TransformComponent* transformCmp = cameraCmp->GetSibling<TransformComponent>();
		if (transformCmp)
		{
			// reinit perspective
			if (cameraCmp->CheckFlags(eComponentBaseFlags::NEWLY_CREATED) || cameraCmp->Aspect != aspect)
			{
				cameraCmp->Aspect = aspect;
				if (cameraCmp->IsPerspective)
					cameraCmp->Projection.SetPerspective(cameraCmp->Fov, cameraCmp->Aspect, cameraCmp->Near, cameraCmp->Far);
				else
					cameraCmp->Projection.SetOrthographic(cameraCmp->Top, cameraCmp->Bottom, cameraCmp->Left, cameraCmp->Right, cameraCmp->Near, cameraCmp->Far);
			}

			cameraCmp->ModelView = transformCmp->GetGlobalTransformationMatrix().GetInversed();
			cameraCmp->MVP = cameraCmp->Projection * cameraCmp->ModelView;
		}
		else
			gConsole.LogError("Entity has camera component but no transform component!");
	}
}
