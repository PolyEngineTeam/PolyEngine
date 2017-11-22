#include "GLRenderingDevice.hpp"

#include <World.hpp>
#include <DebugConfig.hpp>
#include <Viewport.hpp>

#include <CameraComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <TransformComponent.hpp>
#include <ScreenSpaceTextComponent.hpp>
#include <ViewportWorldComponent.hpp>
#include <PostprocessSettingsComponent.hpp>

#include "GLTextFieldBufferDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"
#include "RenderingPassBase.hpp"
#include "GLMeshDeviceProxy.hpp"
#include "GLUtils.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
void GLRenderingDevice::RenderWorld(World* world)
{
	const ScreenSize screenSize = gEngine->GetRenderingDevice()->GetScreenSize();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	// Clear FBO's
	for (eGeometryRenderPassType type : IterateEnum<eGeometryRenderPassType>())
		GeometryRenderingPasses[type]->ClearFBO();

	for (ePostprocessRenderPassType type : IterateEnum<ePostprocessRenderPassType>())
		PostprocessRenderingPasses[type]->ClearFBO();

	// For each visible viewport draw it
	for (auto& kv : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		// Set viewport rect (TOOO change it to propper rect, not box)
		CameraComponent* cameraCmp = kv.second.GetCamera();
		const AARect& rect = kv.second.GetRect();
		const eRenderingModeType renderingMode = cameraCmp->GetRenderingMode();

		glViewport((int)(rect.GetMin().X * screenSize.Width), (int)(rect.GetMin().Y * screenSize.Height),
			(int)(rect.GetSize().X * screenSize.Width), (int)(rect.GetSize().Y * screenSize.Height));

		switch (renderingMode)
		{
			case eRenderingModeType::LIT:
				RenderLit(world, rect, cameraCmp);
				break;

			case eRenderingModeType::UNLIT:
				RenderUnlit(world, rect, cameraCmp);
				break;

			case eRenderingModeType::WIREFRAME:
				RenderWireframe(world, rect, cameraCmp);
				break;

			case eRenderingModeType::DEBUG_NORMALS:
				RenderNormals(world, rect, cameraCmp);
				break;

			case eRenderingModeType::DEBUG_NORMALS_WIREFRAME:
				RenderNormalsWireframe(world, rect, cameraCmp);
				break;

			default:
				ASSERTE(false, "Uknown eRenderingModeType");
		}
	}

	// Signal frame end
	EndFrame();
}

void GLRenderingDevice::RenderWireframe(World* world, const AARect& rect, CameraComponent* cameraCmp) const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	GeometryRenderingPasses[eGeometryRenderPassType::UNLIT]->Run(world, cameraCmp, rect);

	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	GeometryRenderingPasses[eGeometryRenderPassType::TEXT_2D]->Run(world, cameraCmp, rect);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND_LIGHT]->Run(world, cameraCmp, rect);
}

void GLRenderingDevice::RenderNormalsWireframe(World* world, const AARect& rect, CameraComponent* cameraCmp) const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	GeometryRenderingPasses[eGeometryRenderPassType::DEBUG_NORMALS_WIREFRAME]->Run(world, cameraCmp, rect);

	glDepthMask(GL_FALSE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	GeometryRenderingPasses[eGeometryRenderPassType::TEXT_2D]->Run(world, cameraCmp, rect);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND_LIGHT]->Run(world, cameraCmp, rect);
}

void GLRenderingDevice::RenderUnlit(World* world, const AARect& rect, CameraComponent* cameraCmp) const
{
	PostprocessSettingsComponent* post = cameraCmp->GetSibling<PostprocessSettingsComponent>();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	if (post && post->UseBgShader)
		PostprocessRenderingPasses[ePostprocessRenderPassType::BACKGROUND]->Run(world, cameraCmp, rect);
	else
		PostprocessRenderingPasses[ePostprocessRenderPassType::BACKGROUND_LIGHT]->Run(world, cameraCmp, rect);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Render meshes with blin-phong shader
	GeometryRenderingPasses[eGeometryRenderPassType::UNLIT]->Run(world, cameraCmp, rect);

	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	// TODO test these blending options
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

	GeometryRenderingPasses[eGeometryRenderPassType::TRANSPARENT_GEOMETRY]->Run(world, cameraCmp, rect);

	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);

	// Run postprocess passes
	// for (ePostprocessRenderPassType type : IterateEnum<ePostprocessRenderPassType>())

	// Render text
	GeometryRenderingPasses[eGeometryRenderPassType::TEXT_2D]->Run(world, cameraCmp, rect);

	if (post && post->UseFgShader)
		PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND]->Run(world, cameraCmp, rect);
	else
		PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND_LIGHT]->Run(world, cameraCmp, rect);
}

void GLRenderingDevice::RenderNormals(World* world, const AARect& rect, CameraComponent* cameraCmp) const
{
	PostprocessSettingsComponent* post = cameraCmp->GetSibling<PostprocessSettingsComponent>();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	if (post && post->UseBgShader)
		PostprocessRenderingPasses[ePostprocessRenderPassType::BACKGROUND]->Run(world, cameraCmp, rect);
	else
		PostprocessRenderingPasses[ePostprocessRenderPassType::BACKGROUND_LIGHT]->Run(world, cameraCmp, rect);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Render meshes with blin-phong shader
	GeometryRenderingPasses[eGeometryRenderPassType::DEBUG_NORMALS]->Run(world, cameraCmp, rect);

	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	// TODO test these blending options
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

	GeometryRenderingPasses[eGeometryRenderPassType::TRANSPARENT_GEOMETRY]->Run(world, cameraCmp, rect);

	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);

	// Run postprocess passes
	// for (ePostprocessRenderPassType type : IterateEnum<ePostprocessRenderPassType>())

	// Render text
	GeometryRenderingPasses[eGeometryRenderPassType::TEXT_2D]->Run(world, cameraCmp, rect);

	if (post && post->UseFgShader)
		PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND]->Run(world, cameraCmp, rect);
	else
		PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND_LIGHT]->Run(world, cameraCmp, rect);
}

void GLRenderingDevice::RenderLit(World* world, const AARect& rect, CameraComponent* cameraCmp) const
{
	PostprocessSettingsComponent* post = cameraCmp->GetSibling<PostprocessSettingsComponent>();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	if (post && post->UseBgShader)
		PostprocessRenderingPasses[ePostprocessRenderPassType::BACKGROUND]->Run(world, cameraCmp, rect);
	else
		PostprocessRenderingPasses[ePostprocessRenderPassType::BACKGROUND_LIGHT]->Run(world, cameraCmp, rect);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Render meshes with blin-phong shader
	GeometryRenderingPasses[eGeometryRenderPassType::BLINN_PHONG]->Run(world, cameraCmp, rect);

	// Render meshes with unlit shader
	GeometryRenderingPasses[eGeometryRenderPassType::UNLIT]->Run(world, cameraCmp, rect);

	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	// TODO test these blending options
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
	GeometryRenderingPasses[eGeometryRenderPassType::TRANSPARENT_GEOMETRY]->Run(world, cameraCmp, rect);

	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);

	// Run postprocess passes
	// for (ePostprocessRenderPassType type : IterateEnum<ePostprocessRenderPassType>())

	// Render text
	GeometryRenderingPasses[eGeometryRenderPassType::TEXT_2D]->Run(world, cameraCmp, rect);

	if (post && post->UseFgShader)
		PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND]->Run(world, cameraCmp, rect);
	else
		PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND_LIGHT]->Run(world, cameraCmp, rect);
}

void GLRenderingDevice::Init()
{
	InitPrograms();
}
