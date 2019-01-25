#include <EnginePCH.hpp>

#include <Rendering/Camera/CameraSystem.hpp>
#include <Rendering/IRenderingDevice.hpp>
#include <Rendering/ViewportWorldComponent.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <ECS/Scene.hpp>
#include <Input/InputWorldComponent.hpp>

void Poly::CameraSystem::CameraUpdatePhase(Scene* world)
{
	ScreenSize screen = gEngine->GetRenderingDevice()->GetScreenSize();
	for (auto& kv : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		CameraComponent* cameraCmp = kv.second.GetCamera();
		ASSERTE(cameraCmp, "Viewport without camera	?");

		AARect& rect = kv.second.GetRect();
		float aspectWindow = (float)(screen.Width) / (float)(screen.Height);
		float aspectCamera = cameraCmp->GetAspect();

		if (cameraCmp->GetIsForcedRatio()
			&& Abs(aspectCamera - aspectWindow) > 0.001f)
		{
			// if scaled height is less than current height, add letterbox
			float scaleHeight = aspectWindow / aspectCamera;
			if (scaleHeight < 1.0f)
			{
				rect.SetSize(Vector2f(1.0f, scaleHeight));
				rect.SetPosition(Vector2f(0.0f, (1.0f - scaleHeight) / 2.0f));
			}
			else // add pillarbox
			{
				float scaleWidth = 1.0f / scaleHeight;
				rect.SetSize(Vector2f(scaleWidth, 1.0f));
				rect.SetPosition(Vector2f((1.0f - scaleWidth) / 2.0f, 0.0f));
			}
		}
		else
		{
			// reinit perspective
			if (cameraCmp->CheckFlags(eComponentBaseFlags::NEWLY_CREATED)
				|| Abs(aspectCamera - aspectWindow) > 0.001f)
			{
				cameraCmp->SetAspect(aspectWindow);
			}
		}

		EntityTransform& transform = cameraCmp->GetTransform();
		cameraCmp->ViewFromWorld = transform.GetWorldFromModel().GetInversed();
// <<<<<<< HEAD
// 		cameraCmp->ScreenFromWorld = cameraCmp->ScreenFromView * cameraCmp->ViewFromWorld;
// =======
		cameraCmp->ClipFromWorld = cameraCmp->ClipFromView * cameraCmp->ViewFromWorld;
// >>>>>>> dev

		InputWorldComponent* inputCmp = world->GetWorldComponent<InputWorldComponent>();
		if (inputCmp->IsPressed(eKey::F5))
		{
			cameraCmp->SetRenderingMode(eRenderingModeType::LIT);
		}

		if (inputCmp->IsPressed(eKey::F6))
		{
			cameraCmp->SetRenderingMode(eRenderingModeType::UNLIT);
		}

		if (inputCmp->IsPressed(eKey::F7))
		{
			cameraCmp->SetRenderingMode(eRenderingModeType::WIREFRAME);
		}

		if (inputCmp->IsPressed(eKey::F8))
		{
			cameraCmp->SetRenderingMode(eRenderingModeType::DEBUG_NORMALS);
		}

		if (inputCmp->IsPressed(eKey::F9))
		{
			cameraCmp->SetRenderingMode(eRenderingModeType::DEBUG_NORMALS_WIREFRAME);
		}

		if (inputCmp->IsPressed(eKey::F10))
		{
			cameraCmp->SetRenderingMode(eRenderingModeType::IMMEDIATE_DEBUG);
		}
	}
}
