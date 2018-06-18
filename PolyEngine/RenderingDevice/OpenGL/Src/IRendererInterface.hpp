#pragma once

#include <Core.hpp>
#include <Rendering/Viewport.hpp>
#include <Rendering/Lighting/LightSourceComponent.hpp>

// TODO: inherit from BaseRenderPass - make multipass RenderPass

namespace Poly {

	class GLRenderingDevice;

	struct ScreenSize;
	class World;
	class AARect;
	class CameraComponent;
	class MeshRenderingComponent;
	// class DirectionalLightComponent;
	class PointLightComponent;

	struct SceneView : public BaseObject<> {
		SceneView(World* world, const Viewport& viewport)
			: World(world), Viewport(viewport), Rect(viewport.GetRect()), CameraCmp(viewport.GetCamera()) {};

		World* World;
		const Viewport& Viewport;
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