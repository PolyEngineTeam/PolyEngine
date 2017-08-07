#include "GLRenderingDevice.hpp"

#include <World.hpp>
#include <CoreConfig.hpp>
#include <Viewport.hpp>

#include <CameraComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <TransformComponent.hpp>
#include <ScreenSpaceTextComponent.hpp>
#include <ViewportWorldComponent.hpp>

#include "GLTextFieldBufferDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"
#include "GLMeshDeviceProxy.hpp"
#include "GLUtils.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
void GLRenderingDevice::RenderWorld(World * world)
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
		const AABox& rect = kv.second.GetRect();
		glViewport((int)(rect.GetMin().X * screenSize.Width), (int)(rect.GetMin().Y * screenSize.Height),
			(int)(rect.GetSize().X * screenSize.Width), (int)(rect.GetSize().Y * screenSize.Height));

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		// Render meshes with blin-phong shader
		GeometryRenderingPasses[eGeometryRenderPassType::BLINN_PHONG]->Run(world, kv.second.GetCamera(), rect);

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		// Run postprocess passes
		for (ePostprocessRenderPassType type : IterateEnum<ePostprocessRenderPassType>())
			PostprocessRenderingPasses[type]->Run(world, kv.second.GetCamera(), rect);

		// Render text
		GeometryRenderingPasses[eGeometryRenderPassType::TEXT_2D]->Run(world, kv.second.GetCamera(), rect);

		// Draw debug normals
		if (gCoreConfig.DebugNormalsFlag)
			GeometryRenderingPasses[eGeometryRenderPassType::DEBUG_NORMALS]->Run(world, kv.second.GetCamera(), rect);
	}

	// Signal frame end
	EndFrame();
}

