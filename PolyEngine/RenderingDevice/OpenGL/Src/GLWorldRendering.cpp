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

void GLRenderingDevice::Init()
{
	GetExtensions();

	Renderer = CreateRenderer();
	Renderer->Init();

	InitPrograms();
}

ForwardRenderer* GLRenderingDevice::CreateRenderer()
{
	RendererType = OpenGLExtensions.Contains(String("GL_ARB_compute_shader")) ? eRendererType::TILED_FORWARD : eRendererType::FORWARD;
	gConsole.LogInfo("RendererType: {}", (int)RendererType);

	return new ForwardRenderer(this);
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

		PreRender(world, cameraCmp, rect);

		Renderer->Render(world, rect, cameraCmp);

		PostRender(world, cameraCmp, rect);
	}

	// Signal frame end
	EndFrame();
}

void GLRenderingDevice::PreRender(World* world, CameraComponent* cameraCmp, const AARect& rect) const
{
	PostprocessSettingsComponent* post = cameraCmp->GetSibling<PostprocessSettingsComponent>();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	if (post && post->UseBgShader)
		PostprocessRenderingPasses[ePostprocessRenderPassType::BACKGROUND]->Run(world, cameraCmp, rect);
	else
		PostprocessRenderingPasses[ePostprocessRenderPassType::BACKGROUND_LIGHT]->Run(world, cameraCmp, rect);
}

void GLRenderingDevice::PostRender(World* world, CameraComponent* cameraCmp, const AARect& rect) const
{
	PostprocessSettingsComponent* post = cameraCmp->GetSibling<PostprocessSettingsComponent>();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	// Render text
	GeometryRenderingPasses[eGeometryRenderPassType::TEXT_2D]->Run(world, cameraCmp, rect);

	if (post && post->UseFgShader)
		PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND]->Run(world, cameraCmp, rect);
	else
		PostprocessRenderingPasses[ePostprocessRenderPassType::FOREGROUND_LIGHT]->Run(world, cameraCmp, rect);
}
