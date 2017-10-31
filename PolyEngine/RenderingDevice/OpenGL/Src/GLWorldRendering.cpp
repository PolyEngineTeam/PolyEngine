#include "GLRenderingDevice.hpp"

#include <World.hpp>
#include <CoreConfig.hpp>
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

	// Choose correct rendering mode
	glPolygonMode(GL_FRONT_AND_BACK, gCoreConfig.WireframeRendering ? GL_LINE : GL_FILL);
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
		const AARect& rect = kv.second.GetRect();
		PostprocessSettingsComponent* post = kv.second.GetCamera()->GetSibling<PostprocessSettingsComponent>();

		glViewport((int)(rect.GetMin().X * screenSize.Width), (int)(rect.GetMin().Y * screenSize.Height),
			(int)(rect.GetSize().X * screenSize.Width), (int)(rect.GetSize().Y * screenSize.Height));

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		if (post && post->UseBgShader)
			PostprocessRenderingPasses[ePostprocessRenderPassType::BACKGROUND]->Run(world, kv.second.GetCamera(), rect);
		else
			PostprocessRenderingPasses[ePostprocessRenderPassType::BACKGROUND_LIGHT]->Run(world, kv.second.GetCamera(), rect);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		// Render meshes with blin-phong shader
		GeometryRenderingPasses[eGeometryRenderPassType::BLINN_PHONG]->Run(world, kv.second.GetCamera(), rect);

		glDepthMask(GL_FALSE);
		
		glEnable(GL_BLEND);

		// TODO test these blending options
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_ONE, GL_ONE);
		//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

		GeometryRenderingPasses[eGeometryRenderPassType::TRANSPARENT_GEOMETRY]->Run(world, kv.second.GetCamera(), rect);

		glDisable(GL_BLEND);

		glDisable(GL_DEPTH_TEST);

		// Run postprocess passes
		// for (ePostprocessRenderPassType type : IterateEnum<ePostprocessRenderPassType>())

		// Render text
		GeometryRenderingPasses[eGeometryRenderPassType::TEXT_2D]->Run(world, kv.second.GetCamera(), rect);

		if (post && post->UseFgShader)
			PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND]->Run(world, kv.second.GetCamera(), rect);
		else 
		 	PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND_LIGHT]->Run(world, kv.second.GetCamera(), rect);

		// Draw debug normals
		if (gCoreConfig.DebugNormalsFlag)
			GeometryRenderingPasses[eGeometryRenderPassType::DEBUG_NORMALS]->Run(world, kv.second.GetCamera(), rect);
	}

	// Signal frame end
	EndFrame();
}

void Poly::GLRenderingDevice::Init()
{
	InitPrograms();
}

