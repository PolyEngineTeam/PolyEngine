#include "ForwardRenderer.hpp"

#include "GLRenderingDevice.hpp"
#include "Pipeline/RenderingPassBase.hpp"

#include <Rendering/Camera/CameraComponent.hpp>
#include <Math/AARect.hpp>

using namespace Poly;

ForwardRenderer::ForwardRenderer(GLRenderingDevice* RenderingDeviceInterface)
	: IRendererInterface(RenderingDeviceInterface)
{
	gConsole.LogInfo("ForwardRenderer::ForwardRenderer");
}

void ForwardRenderer::Init()
{
	gConsole.LogInfo("ForwardRenderer::Init");
}

void ForwardRenderer::Resize(const ScreenSize & size)
{
	gConsole.LogInfo("ForwardRenderer::Resize");
}

void ForwardRenderer::Deinit()
{
	gConsole.LogInfo("ForwardRenderer::Deinit");
}

void ForwardRenderer::Render(const SceneView& sceneView)
{
	gConsole.LogInfo("ForwardRenderer::Render");

	const eRenderingModeType renderingMode = sceneView.cameraCmp->GetRenderingMode();

	switch (renderingMode)
	{
		case eRenderingModeType::LIT:
			RenderLit(sceneView.world, sceneView.rect, sceneView.cameraCmp);
			break;

		case eRenderingModeType::UNLIT:
			RenderUnlit(sceneView.world, sceneView.rect, sceneView.cameraCmp);
			break;

		case eRenderingModeType::WIREFRAME:
			RenderWireframe(sceneView.world, sceneView.rect, sceneView.cameraCmp);
			break;

		case eRenderingModeType::DEBUG_NORMALS:
			RenderNormals(sceneView.world, sceneView.rect, sceneView.cameraCmp);
			break;

		case eRenderingModeType::DEBUG_NORMALS_WIREFRAME:
			RenderNormalsWireframe(sceneView.world, sceneView.rect, sceneView.cameraCmp);
			break;

		default:
			ASSERTE(false, "Uknown eRenderingModeType");
	}

	PostRender(sceneView.world, sceneView.cameraCmp, sceneView.rect);
}

void ForwardRenderer::PostRender(World* world, const CameraComponent* cameraCmp, const AARect& rect)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::SKYBOX]->Run(world, cameraCmp, rect);
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::TEXT_2D]->Run(world, cameraCmp, rect);
	RDI->PostprocessRenderingPasses[GLRenderingDevice::ePostprocessRenderPassType::FOREGROUND_LIGHT]->Run(world, cameraCmp, rect);
}

void ForwardRenderer::RenderLit(World* world, const AARect& rect, const CameraComponent* cameraCmp) const
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

void ForwardRenderer::RenderUnlit(World* world, const AARect& rect, const CameraComponent* cameraCmp) const
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Render meshes with blin-phong shader
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::UNLIT]->Run(world, cameraCmp, rect, ePassType::GLOBAL);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	// TODO test these blending options
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::TRANSPARENT_GEOMETRY]->Run(world, cameraCmp, rect);
}

void ForwardRenderer::RenderWireframe(World* world, const AARect& rect, const CameraComponent* cameraCmp) const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::UNLIT]->Run(world, cameraCmp, rect, ePassType::GLOBAL);

	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::TEXT_2D]->Run(world, cameraCmp, rect);
}

void ForwardRenderer::RenderNormals(World* world, const AARect& rect, const CameraComponent* cameraCmp) const
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Render meshes with blin-phong shader
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::DEBUG_NORMALS]->Run(world, cameraCmp, rect, ePassType::GLOBAL);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	// TODO test these blending options
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::TRANSPARENT_GEOMETRY]->Run(world, cameraCmp, rect);
}

void ForwardRenderer::RenderNormalsWireframe(World* world, const AARect& rect, const CameraComponent* cameraCmp) const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::DEBUG_NORMALS_WIREFRAME]->Run(world, cameraCmp, rect, ePassType::GLOBAL);

	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	RDI->GeometryRenderingPasses[GLRenderingDevice::eGeometryRenderPassType::TEXT_2D]->Run(world, cameraCmp, rect);
}