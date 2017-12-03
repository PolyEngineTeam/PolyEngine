#pragma once

#include <Core.hpp>


namespace Poly {

	class GLRenderingDevice;

	class CameraComponent;
	class World;
	class AARect;

	class ForwardRenderer : public BaseObject<>
	{
	public:
		ForwardRenderer(GLRenderingDevice* RenderingDeviceInterface);

		void Init();
		void Render(World* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void Deinit();

	private:
		
		GLRenderingDevice* RDI;

		void RenderLit(World* world, const AARect& rect, CameraComponent* cameraCmp) const;
	};
}
