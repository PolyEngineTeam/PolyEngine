#pragma once

#include <pe/Defines.hpp>
#include <IRendererInterface.hpp>

namespace Poly {

	class ForwardRenderer : public IRendererInterface
	{
	public:
		ForwardRenderer(GLRenderingDevice* RenderingDeviceInterface);

		void Init() override;
		void Resize(const ScreenSize& size) override;
		void Render(const SceneView& sceneView) override;
		void Deinit() override;

		void PostRender(Scene* world, const CameraComponent* cameraCmp, const AARect& rect);

	private:
		void RenderLit(Scene* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderUnlit(Scene* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderWireframe(Scene* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderNormals(Scene* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderNormalsWireframe(Scene* world, const AARect& rect, const CameraComponent* cameraCmp) const;
	};
}
