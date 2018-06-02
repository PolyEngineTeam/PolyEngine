// #include "ForwardRenderer.hpp"

#include "GLRenderingDevice.hpp"
#include "Pipeline/RenderingPassBase.hpp"

#include <Rendering/Camera/CameraComponent.hpp>
#include <Math/AARect.hpp>
#include "IRendererInterface.hpp"

using namespace Poly;

IRendererInterface::IRendererInterface(GLRenderingDevice* RenderingDeviceInterface)
	: RDI(RenderingDeviceInterface)
{
	ASSERTE(RenderingDeviceInterface != nullptr, "RenderingDeviceInterface passed to Renderer is NULL");
}
