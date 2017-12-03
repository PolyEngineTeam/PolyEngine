#pragma once

#include "IRendererInterface.hpp"

namespace Poly {

	class TiledForwardRenderer : public IRendererInterface
	{
	public:
		TiledForwardRenderer(GLRenderingDevice* RenderingDeviceInterface);

		void Init() override;
		void Render(World* world, const AARect& rect, CameraComponent* cameraCmp) const override;
		void Deinit() override;

	private:
		void RenderLit(World* world, const AARect& rect, CameraComponent* cameraCmp) const;
	};
}