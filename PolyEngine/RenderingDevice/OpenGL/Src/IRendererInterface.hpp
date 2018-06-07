#pragma once

#include <Core.hpp>
#include <Rendering/Viewport.hpp>

// TODO: inherit from BaseRenderPass - make multipass RenderPass

namespace Poly {

	class GLRenderingDevice;

	class CameraComponent;
	class World;
	class AARect;
	class MeshRenderingComponent;
	class DirectionalLightComponent;
	class PointLightComponent;

	struct SceneView : public BaseObject<> {
		SceneView(World* world, const Viewport& viewport)
			: world(world), viewport(viewport), rect(viewport.GetRect()), cameraCmp(viewport.GetCamera()) {};

		World* world;
		const Viewport& viewport;
		const AARect& rect;
		const CameraComponent* cameraCmp;

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
		virtual void Render(const SceneView& sceneView) = 0;
		virtual void Deinit() = 0;

	protected:
		GLRenderingDevice* RDI;
	};
}