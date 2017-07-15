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
	// Prepare frame buffer
	glDepthMask(GL_TRUE);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Choose correct rendering mode
	if (gCoreConfig.WireframeRendering)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	ScreenSize screen = gEngine->GetRenderingDevice()->GetScreenSize();

	// For each visible viewport draw it
	for (auto& kv : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		// Get viewport rect (TOOO change it to propper rect, not box)
		const AABox& rect = kv.second.GetRect();
		glViewport((int)(rect.GetMin().X * screen.Width), (int)(rect.GetMin().Y * screen.Height),
			(int)(rect.GetSize().X * screen.Width), (int)(rect.GetSize().Y * screen.Height));

		// Render meshes
		RootRenderingPasses[eRootRenderPassType::BLINN_PHONG]->Run(world, kv.second.GetCamera(), rect);

		// Draw debug normals
		if (gCoreConfig.DebugNormalsFlag)
		{
			RootRenderingPasses[eRootRenderPassType::DEBUG_NORMALS]->Run(world, kv.second.GetCamera(), rect);
		}

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		RootRenderingPasses[eRootRenderPassType::TEXT_2D]->Run(world, kv.second.GetCamera(), rect);
	}

	// Signal frame end
	EndFrame();
}

