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

		void PostRender(Scene* world, const CameraComponent* cameraCmp, const core::math::AARect& rect);

	private:
		void RenderLit(Scene* world, const core::math::AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderUnlit(Scene* world, const core::math::AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderWireframe(Scene* world, const core::math::AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderNormals(Scene* world, const core::math::AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderNormalsWireframe(Scene* world, const core::math::AARect& rect, const CameraComponent* cameraCmp) const;
	};
}
