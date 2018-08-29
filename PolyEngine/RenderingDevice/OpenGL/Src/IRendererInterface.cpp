#include "PolyRenderingDeviceGLPCH.hpp"
// #include "ForwardRenderer.hpp"

#include "IRendererInterface.hpp"
#include "GLRenderingDevice.hpp"
#include "Pipeline/RenderingPassBase.hpp"

using namespace Poly;

IRendererInterface::IRendererInterface(GLRenderingDevice* RenderingDeviceInterface)
	: RDI(RenderingDeviceInterface)
{
	ASSERTE(RenderingDeviceInterface != nullptr, "RenderingDeviceInterface passed to Renderer is NULL");
}
