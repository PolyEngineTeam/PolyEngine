#pragma once

#include "IRendererInterface.hpp"

namespace Poly {

	class ForwardRenderer : public IRendererInterface
	{
	public:
		ForwardRenderer(GLRenderingDevice* RenderingDeviceInterface);

		void Init() override;
		void Render(World* world, const AARect& rect, const CameraComponent* cameraCmp) override;
		void PostRender(Poly::World * &world, const Poly::CameraComponent * &cameraCmp, const Poly::AARect & rect);
		void Deinit() override;

	private:
		void RenderLit(World* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderUnlit(World* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderWireframe(World* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderNormals(World* world, const AARect& rect, const CameraComponent* cameraCmp) const;
		void RenderNormalsWireframe(World* world, const AARect& rect, const CameraComponent* cameraCmp) const;
	};
}
