#include "ForwardRenderer.hpp"

#include "GLRenderingDevice.hpp"
#include "RenderingPassBase.hpp"

#include <CameraComponent.hpp>
#include <AARect.hpp>

using namespace Poly;

ForwardRenderer::ForwardRenderer(GLRenderingDevice* RenderingDeviceInterface)
	: RDI(RenderingDeviceInterface)
{
}

void ForwardRenderer::Init()
{
}

void ForwardRenderer::Render(World* world, const AARect& rect, CameraComponent* cameraCmp) const
{
	const eRenderingModeType renderingMode = cameraCmp->GetRenderingMode();

	switch (renderingMode)
	{
		case eRenderingModeType::LIT:
			RenderLit(world, rect, cameraCmp);
			break;
// 
// 		case eRenderingModeType::UNLIT:
// 			RenderUnlit(world, rect, cameraCmp);
// 			break;
// 
// 		case eRenderingModeType::WIREFRAME:
// 			RenderWireframe(world, rect, cameraCmp);
// 			break;
// 
// 		case eRenderingModeType::DEBUG_NORMALS:
// 			RenderNormals(world, rect, cameraCmp);
// 			break;
// 
// 		case eRenderingModeType::DEBUG_NORMALS_WIREFRAME:
// 			RenderNormalsWireframe(world, rect, cameraCmp);
// 			break;

		default:
			ASSERTE(false, "Uknown eRenderingModeType");
	}
}

void ForwardRenderer::RenderLit(World* world, const AARect& rect, CameraComponent* cameraCmp) const
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Render meshes with blin-phong shader
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::BLINN_PHONG]->Run(world, cameraCmp, rect);

	// Render meshes with unlit shader
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::UNLIT]->Run(world, cameraCmp, rect);

	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}

void ForwardRenderer::Deinit()
{
}