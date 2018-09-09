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
		const AARect& rect = kv.second.GetRect();
		float aspect = (rect.GetSize().X * screen.Width) / (rect.GetSize().Y * screen.Height);

		CameraComponent* cameraCmp = kv.second.GetCamera();
		ASSERTE(cameraCmp, "Viewport without camera?");
		EntityTransform& transform = cameraCmp->GetTransform();

		// reinit perspective
		if (cameraCmp->CheckFlags(eComponentBaseFlags::NEWLY_CREATED) || cameraCmp->GetAspect() != aspect)
		{
			cameraCmp->SetAspect(aspect);
		}

		cameraCmp->ViewFromWorld = transform.GetWorldFromModel().GetInversed();
		cameraCmp->ScreenFromWorld = cameraCmp->ScreenFromView * cameraCmp->ViewFromWorld;


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
