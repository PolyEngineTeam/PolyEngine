#pragma once

#include <Core.hpp>


namespace Poly {

	class GLRenderingDevice;

	class CameraComponent;
	class World;
	class AARect;

	class IRendererInterface : public BaseObject<>
	{
	public:
		IRendererInterface(GLRenderingDevice* RenderingDeviceInterface);

		virtual void Init() = 0;
		virtual void Render(World* world, const AARect& rect, CameraComponent* cameraCmp) const = 0;
		virtual void Deinit() = 0;

	protected:
		GLRenderingDevice* RDI;
	};
}