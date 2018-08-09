#pragma once

#include <Core.hpp>
#include <Rendering/Viewport.hpp>
#include <Rendering/Lighting/LightSourceComponent.hpp>

// TODO: inherit from BaseRenderPass - make multipass RenderPass

namespace Poly {

	class GLRenderingDevice;

	struct ScreenSize;
	class Scene;
	class AARect;
	class CameraComponent;
	class MeshRenderingComponent;

	struct SceneView : public BaseObject<> {
		SceneView(Scene* w, const Viewport& v)
			: WorldData(w), ViewportData(v), Rect(v.GetRect()), CameraCmp(v.GetCamera())
		{};

		Scene* WorldData;
		const Viewport& ViewportData;
		const AARect& Rect;
		const CameraComponent* CameraCmp;

		Dynarray<const MeshRenderingComponent*> OpaqueQueue;
		Dynarray<const MeshRenderingComponent*> TranslucentQueue;

		Dynarray<const DirectionalLightComponent*> DirectionalLights;
		Dynarray<const PointLightComponent*> PointLights;
	};

	class IRendererInterface : public BaseObject<>
	{
	public:
		IRendererInterface(GLRenderingDevice* RenderingDeviceInterface);

		virtual void Init() = 0;		
		virtual void Resize(const ScreenSize& size) = 0;
		virtual void Render(const SceneView& sceneView) = 0;
		virtual void Deinit() = 0;

	protected:
		GLRenderingDevice* RDI;
	};
}