#include "GLRenderingDevice.hpp"

#include <World.hpp>
#include <DebugConfig.hpp>
#include <Viewport.hpp>

#include <CameraComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <ScreenSpaceTextComponent.hpp>
#include <ViewportWorldComponent.hpp>
#include <PostprocessSettingsComponent.hpp>

#include "GLTextFieldBufferDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"
#include "RenderingPassBase.hpp"
#include "GLMeshDeviceProxy.hpp"
#include "ForwardRenderer.hpp"
#include "TiledForwardRenderer.hpp"
#include "GLUtils.hpp"

using namespace Poly;

//------------------------------------------------------------------------------

void GLRenderingDevice::Init()
{
	GetExtensions();

	Renderer = CreateRenderer();
	Renderer->Init();

	InitPrograms();
}

IRendererInterface* GLRenderingDevice::CreateRenderer()
{
	RendererType = OpenGLExtensions.Contains(String("GL_ARB_compute_shader")) ? eRendererType::TILED_FORWARD : eRendererType::FORWARD;
	gConsole.LogInfo("RendererType: {}", (int)RendererType);

	IRendererInterface* renderer = nullptr;

	switch (RendererType)
	{
		case GLRenderingDevice::eRendererType::FORWARD:
			renderer = new ForwardRenderer(this);
			break;

		case GLRenderingDevice::eRendererType::TILED_FORWARD:
			renderer = new TiledForwardRenderer(this);
			break;

		default:
			ASSERTE(false, "Uknown eRenderingModeType");
	}

	return renderer;
}

void GLRenderingDevice::Deinit()
{
	Renderer->Deinit();
}

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

		glViewport((int)(rect.GetMin().X * screenSize.Width), (int)(rect.GetMin().Y * screenSize.Height),
			(int)(rect.GetSize().X * screenSize.Width), (int)(rect.GetSize().Y * screenSize.Height));

		Renderer->Render(world, rect, cameraCmp);
	}

	// Signal frame end
	EndFrame();
}
