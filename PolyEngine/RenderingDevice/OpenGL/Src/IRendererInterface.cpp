#include "ForwardRenderer.hpp"

#include "GLRenderingDevice.hpp"
#include "RenderingPassBase.hpp"

#include <CameraComponent.hpp>
#include <AARect.hpp>
#include "IRendererInterface.hpp"

using namespace Poly;

IRendererInterface::IRendererInterface(GLRenderingDevice* RenderingDeviceInterface)
	: RDI(RenderingDeviceInterface)
{
	ASSERTE(RenderingDeviceInterface != nullptr, "RenderingDeviceInterface passed to Renderer is NULL");
}
