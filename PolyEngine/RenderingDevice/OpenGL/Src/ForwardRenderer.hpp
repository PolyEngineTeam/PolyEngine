#pragma once

#include "IRendererInterface.hpp"

namespace Poly {

	class ForwardRenderer : public IRendererInterface
	{
	public:
		ForwardRenderer(GLRenderingDevice* RenderingDeviceInterface);

		void Init() override;
		void Render(const SceneView& sceneView) override;
		void Deinit() override;

		void PostRender(World* world, const CameraComponent* cameraCmp, const AARect& rect);

	private:
		void RenderLit(World* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderUnlit(World* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderWireframe(World* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderNormals(World* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderNormalsWireframe(World* world, const AARect& rect, const CameraComponent* cameraCmp) const;
	};
}
