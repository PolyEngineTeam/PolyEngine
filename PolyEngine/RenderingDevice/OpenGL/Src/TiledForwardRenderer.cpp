#include "TiledForwardRenderer.hpp"

#include "GLRenderingDevice.hpp"
#include "RenderingPassBase.hpp"

#include <CameraComponent.hpp>
#include <AARect.hpp>

using namespace Poly;

TiledForwardRenderer::TiledForwardRenderer(GLRenderingDevice* RenderingDeviceInterface)
	: IRendererInterface(RenderingDeviceInterface)
{
}

void TiledForwardRenderer::Init()
{
}

void TiledForwardRenderer::Deinit()
{
}

void TiledForwardRenderer::Render(World* world, const AARect& rect, CameraComponent* cameraCmp) const
{
	const eRenderingModeType renderingMode = cameraCmp->GetRenderingMode();

	switch (renderingMode)
	{
		case eRenderingModeType::LIT:
  			RenderLit(world, rect, cameraCmp);
 			break;

		default:
			ASSERTE(false, "Uknown eRenderingModeType");
	}
}

void TiledForwardRenderer::RenderLit(World* world, const AARect& rect, CameraComponent* cameraCmp) const
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Render meshes with blin-phong shader
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::BLINN_PHONG]->Run(world, cameraCmp, rect);

	// Render meshes with unlit shader
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::UNLIT]->Run(world, cameraCmp, rect);
}