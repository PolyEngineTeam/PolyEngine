#include <PolyRenderingDeviceGLPCH.hpp>

#include <TiledForwardRenderer.hpp>
#include <Configs/AssetsPathConfig.hpp>
#include <Rendering/Animation/SkeletalAnimationComponent.hpp>

#include <GLRenderingDevice.hpp>
#include <Proxy/GLMeshDeviceProxy.hpp>
#include <Proxy/GLParticleDeviceProxy.hpp>
#include <Pipeline/RenderingPassBase.hpp>
#include <Common/DebugRenderingBuffers.hpp>
#include <Proxy/GLShaderProgram.hpp>
#include <Common/PrimitiveCube.hpp>
#include <Common/PrimitiveQuad.hpp>
#include <Debugging/DebugDrawSystem.hpp>

#include <Proxy/imgui_impl_opengl3.h>

using namespace Poly;
using MeshQueue = core::storage::PriorityQueue<const MeshRenderingComponent*, SceneView::DistanceToCameraComparator>;

void RenderTargetPingPong::Init(int width, int height)
{
	Width = width;
	Height = height;

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongTarget);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongTarget[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Width, Height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongTarget[i], 0);
	}
}

void RenderTargetPingPong::Deinit()
{
	glDeleteFramebuffers(2, pingpongFBO);
	glDeleteTextures(2, pingpongTarget);
}


TiledForwardRenderer::TiledForwardRenderer(GLRenderingDevice* rdi)
	: IRendererInterface(rdi),
	LastViewportRect(core::math::Vector2f::ZERO, core::math::Vector2f::ONE),
	ShadowMap(rdi),
	EnvironmentCapture(rdi),
	DepthShader("Shaders/depth.vert.glsl", "Shaders/depth.frag.glsl"),
	LightCullingShader("Shaders/lightCulling.comp.glsl"),
	LightAccumulationShader("Shaders/lightAccumulation.vert.glsl", "Shaders/lightAccumulation.frag.glsl"),
	HDRShader("Shaders/hdr.vert.glsl", "Shaders/hdr.frag.glsl"),
	SkyboxShader("Shaders/skybox.vert.glsl", "Shaders/skybox.frag.glsl"),
	LinearizeDepthShader("Shaders/hdr.vert.glsl", "Shaders/linearizeDepth.frag.glsl"),
	FXAAShader("Shaders/hdr.vert.glsl", "Shaders/fxaa.frag.glsl"),
	GammaShader("Shaders/hdr.vert.glsl", "Shaders/gamma.frag.glsl"),
	MotionBlurShader("Shaders/hdr.vert.glsl", "Shaders/motionblur.frag.glsl"),
	DOFBokehShader("Shaders/hdr.vert.glsl", "Shaders/dof_pass_bokeh.frag.glsl"),
	DOFApplyShader("Shaders/hdr.vert.glsl", "Shaders/dof_pass_apply.frag.glsl"),
	BloomBrightShader("Shaders/hdr.vert.glsl", "Shaders/bloom_pass_bright.frag.glsl"),
	BloomBlurShader("Shaders/hdr.vert.glsl", "Shaders/bloom_pass_blur.frag.glsl"),
	BloomApplyShader("Shaders/hdr.vert.glsl", "Shaders/bloom_pass_apply.frag.glsl"),
	ParticleShader("Shaders/instanced.vert.glsl", "Shaders/instanced.frag.glsl"),
	TranslucentShader("Shaders/translucent.vert.glsl", "Shaders/translucent.frag.glsl"),
	EquiToCubemapShader("Shaders/equiHdr.vert.glsl", "Shaders/equiHdr.frag.glsl"),
	IntegrateBRDFShader("Shaders/hdr.vert.glsl", "Shaders/integrateBRDF.frag.glsl"),
	Text2DShader("Shaders/text2D.vert.glsl", "Shaders/text2D.frag.glsl"),
	EditorDebugShader("Shaders/debug.vert.glsl", "Shaders/debug.frag.glsl"),
	DebugQuadDepthPrepassShader("Shaders/debugQuadDepthPrepass.vert.glsl", "Shaders/debugQuadDepthPrepass.frag.glsl"),
	DebugQuadLightCullingShader("Shaders/debugQuadLightCulling.vert.glsl", "Shaders/debugQuadLightCulling.frag.glsl"),
	DebugLightAccumShader("Shaders/debugLightAccum.vert.glsl", "Shaders/debugLightAccum.frag.glsl"),
	DebugTextureInputsShader("Shaders/lightAccumulation.vert.glsl", "Shaders/lightAccumulationTexDebug.frag.glsl")
{
	LightAccumulationShader.RegisterUniform("vec4", "uMaterial.Emissive");
	LightAccumulationShader.RegisterUniform("vec4", "uMaterial.Albedo");
	LightAccumulationShader.RegisterUniform("float", "uMaterial.Roughness");
	LightAccumulationShader.RegisterUniform("float", "uMaterial.Metallic");
	LightAccumulationShader.RegisterUniform("float", "uMaterial.OpacityMaskThreshold");
	  
	for (int i = 0; i < 8; ++i)
	{
		core::storage::String baseName = core::storage::String("uDirectionalLight[") + core::storage::String::From(i) + core::storage::String("].");
		LightAccumulationShader.RegisterUniform("vec4", baseName + "ColorIntensity");
		LightAccumulationShader.RegisterUniform("vec4", baseName + "Direction");
	}
	
	int maxBones = 64;
	for (int i = 0; i < maxBones; ++i)
	{
		core::storage::String baseName = core::storage::String("uBones[") + core::storage::String::From(i) + core::storage::String("]");
		LightAccumulationShader.RegisterUniform("mat4", baseName);
	}

	TranslucentShader.RegisterUniform("vec4", "uMaterial.Emissive");
	TranslucentShader.RegisterUniform("vec4", "uMaterial.Albedo");
	TranslucentShader.RegisterUniform("float", "uMaterial.Roughness");
	TranslucentShader.RegisterUniform("float", "uMaterial.Metallic");
	TranslucentShader.RegisterUniform("float", "uMaterial.OpacityMaskThreshold");

	for (int i = 0; i < 8; ++i)
	{
		core::storage::String baseName = core::storage::String("uDirectionalLight[") + core::storage::String::From(i) + core::storage::String("].");
		TranslucentShader.RegisterUniform("vec4", baseName + "ColorIntensity");
		TranslucentShader.RegisterUniform("vec4", baseName + "Direction");
	}
}

void TiledForwardRenderer::Init()
{
	core::utils::gConsole.LogInfo("TiledForwardRenderer::Init");
	
	const ScreenSize screenSize = RDI->GetScreenSize();
	core::math::Vector2f viewportSize = LastViewportRect.GetSize();
	ScreenSize targetsSize;
	targetsSize.Width = (int)(viewportSize.X * screenSize.Width);
	targetsSize.Height = (int)(viewportSize.Y * screenSize.Height);

	core::utils::gConsole.LogInfo("TiledForwardRenderer::Init screenSize: ({},{}), targetSize: ({},{}),",
		screenSize.Width, screenSize.Height, targetsSize.Width, targetsSize.Height);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	CreateRenderTargets(screenSize);
	CreateLightBuffers(screenSize);
	CapturePreintegratedBRDF();
	SetupLightsBufferFromScene();

	Splash = ResourceManager<TextureResource>::Load("Textures/splash_00.png", eResourceSource::ENGINE, eTextureUsageType::ALBEDO);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void TiledForwardRenderer::Resize(const ScreenSize& size)
{
	core::utils::gConsole.LogInfo("TiledForwardRenderer::Resize ({}, {}), LastRect: {}", size.Width, size.Height, LastViewportRect);
	
	core::math::Vector2f ViewportSize = LastViewportRect.GetSize();
	ScreenSize targetsSize;
	targetsSize.Width = (int)(ViewportSize.X * size.Width);
	targetsSize.Height = (int)(ViewportSize.Y * size.Height);
	
	DeleteLightBuffers();
	DeleteRenderTargets();
	CreateLightBuffers(targetsSize);
	CreateRenderTargets(targetsSize);
}

void TiledForwardRenderer::Deinit()
{
	core::utils::gConsole.LogInfo("TiledForwardRenderer::Deinit");
	
	ShadowMap.Deinit();

	if (ImGui::GetCurrentContext() != nullptr)
	{
		// Imgui context is needed to propertly deinit textures.
		// Deinit is called after engine resources we need  destroy
		// imgui pass textures and objects and then end Imgui context
		// started in ImguiResource owned by ImguiWorldComponent
		ImGui_ImplOpenGL3_DestroyDeviceObjects();
		ImGui::DestroyContext();
	}

	DeleteLightBuffers();

	DeleteRenderTargets();

	if (Splash)
	{
		ResourceManager<TextureResource>::Release(Splash);
	}
}

void TiledForwardRenderer::CapturePreintegratedBRDF()
{
	// Create texture resource
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::PreintegrateBRDF Create texture resource");
	glGenTextures(1, &PreintegratedBrdfLUT);
	glBindTexture(GL_TEXTURE_2D, PreintegratedBrdfLUT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	// CHECK_GL_ERR();

	// Create FBO for BRDF preintegration
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::PreintegrateBRDF Create capture FBO");
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::PreintegrateBRDF Capture FBO created");

	// Render preintegrated BRDF
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::PreintegrateBRDF Render preintegrated BRDF");
	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	IntegrateBRDFShader.BindProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PreintegratedBrdfLUT, 0);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	core::utils::gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap preintegrated BRDF rendered");
}

void TiledForwardRenderer::CreateLightBuffers(const ScreenSize& size)
{
	// Define work group sizes in x and y direction based off screen size and tile size (in pixels)
	WorkGroupsX = (size.Width + (size.Width % 16)) / 16;
	WorkGroupsY = (size.Height + (size.Height % 16)) / 16;
	size_t numberOfTiles = WorkGroupsX * WorkGroupsY;
	core::utils::gConsole.LogInfo("TiledForwardRenderer::Init workGroups: ({},{}), numberOfTiles: {}", WorkGroupsX, WorkGroupsY, numberOfTiles);

	// Generate our shader storage buffers
	glGenBuffers(1, &LightBuffer);
	glGenBuffers(1, &VisibleLightIndicesBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * MAX_NUM_LIGHTS, 0, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VisibleLightIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VisibleIndex) * MAX_NUM_LIGHTS * numberOfTiles, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	CHECK_GL_ERR();
}

void TiledForwardRenderer::DeleteLightBuffers()
{
	if(LightBuffer > 0)
		glDeleteBuffers(1, &LightBuffer);

	if(VisibleLightIndicesBuffer > 0)
		glDeleteBuffers(1, &VisibleLightIndicesBuffer);
}

void TiledForwardRenderer::CreateRenderTargets(const ScreenSize& size)
{
	GLuint screenSizeX = size.Width;
	GLuint screenSizeY = size.Height;

	glGenFramebuffers(1, &FBOdepthMap);
	glGenTextures(1, &PreDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, PreDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenSizeX, screenSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOdepthMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, PreDepthBuffer, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK_GL_ERR();
	CHECK_FBO_STATUS();

	// Create a floating point HDR frame buffer and a floating point color buffer (as a texture)
	glGenFramebuffers(1, &FBOhdr);

	glGenTextures(1, &ColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenSizeX, screenSizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &NormalBuffer);
	glBindTexture(GL_TEXTURE_2D, NormalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenSizeX, screenSizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// It will also need a depth component as a render buffer, attached to the hdrFBO
	glGenRenderbuffers(1, &RboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, RboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenSizeX, screenSizeY);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBuffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NormalBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RboDepth);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK_GL_ERR();
	CHECK_FBO_STATUS();

	// Create pair of frame buffers for post process to swap
	glGenFramebuffers(1, &FBOpost0);

	glGenTextures(1, &PostColorBuffer0);
	glBindTexture(GL_TEXTURE_2D, PostColorBuffer0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenSizeX, screenSizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostColorBuffer0, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK_GL_ERR();
	CHECK_FBO_STATUS();

	glGenFramebuffers(1, &FBOpost1);

	glGenTextures(1, &PostColorBuffer1);
	glBindTexture(GL_TEXTURE_2D, PostColorBuffer1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenSizeX, screenSizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostColorBuffer1, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &LinearDepth);
	glBindTexture(GL_TEXTURE_2D, LinearDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenSizeX, screenSizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &PostColorBufferHalfRes);
	glBindTexture(GL_TEXTURE_2D, PostColorBufferHalfRes);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenSizeX / 2, screenSizeY / 2, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	RTBloom.Init(screenSizeX / 2, screenSizeY / 2);

	CHECK_GL_ERR();
	CHECK_FBO_STATUS();
}

void TiledForwardRenderer::DeleteRenderTargets()
{
	if (PreDepthBuffer > 0)
		glDeleteTextures(1, &PreDepthBuffer);

	if (FBOdepthMap > 0)
		glDeleteFramebuffers(1, &FBOdepthMap);

	if (ColorBuffer > 0)
		glDeleteTextures(1, &ColorBuffer);

	if (NormalBuffer > 0)
		glDeleteTextures(1, &NormalBuffer);

	if (RboDepth > 0)
		glDeleteRenderbuffers(1, &RboDepth);

	if (FBOhdr > 0)
		glDeleteFramebuffers(1, &FBOhdr);

	if (PostColorBuffer0 > 0)
		glDeleteTextures(1, &PostColorBuffer0);

	if (FBOpost0 > 0)
		glDeleteFramebuffers(1, &FBOpost0);

	if (PostColorBuffer1 > 0)
		glDeleteTextures(1, &PostColorBuffer1);

	if (PostColorBufferHalfRes > 0)
		glDeleteTextures(1, &PostColorBufferHalfRes);

	if (LinearDepth > 0)
		glDeleteTextures(1, &LinearDepth);

	if (FBOpost1 > 0)
		glDeleteFramebuffers(1, &FBOpost1);

	RTBloom.Deinit();

	CHECK_FBO_STATUS();
}

void TiledForwardRenderer::Render(const SceneView& sceneView)
{
	if (LastViewportRect != sceneView.Rect)
	{
		LastViewportRect = sceneView.Rect;
		Resize(RDI->GetScreenSize());
	}

	static bool isInitOnFirstFrame = false;

	// glViewport((int)(sceneView.Rect.GetMin().X * screenSize.Width), (int)(sceneView.Rect.GetMin().Y * screenSize.Height),
	// 	(int)(sceneView.Rect.size().X * screenSize.Width), (int)(sceneView.Rect.size().Y * screenSize.Height));

	if (!isInitOnFirstFrame)
	{
		ShadowMap.Init(sceneView);
		UpdateEnvCapture(sceneView);
		
		isInitOnFirstFrame = true;
	}
	
	UpdateLightsBufferFromScene(sceneView);
	ShadowMap.Render(sceneView);
	RenderDepthPrePass(sceneView);
	ComputeLightCulling(sceneView);
	RenderOpaqueLit(sceneView);
	RenderSkybox(sceneView);
	RenderTranslucentLit(sceneView);
	RenderParticleUnlit(sceneView);
	PostLinearizeDepth(sceneView);
	PostMotionBlur(sceneView);
	PostDepthOfField(sceneView);
	PostBloom(sceneView);
	PostTonemapper(sceneView);
	PostFXAA(sceneView);
	PostGamma(sceneView);
	EditorDebug(sceneView);
	UIText2D(sceneView);
	UIImgui();

	// ensure that copy of matrix is stored
	PreviousFrameCameraClipFromWorld = core::math::Matrix(sceneView.CameraCmp->GetClipFromWorld().GetDataPtr());
	PreviousFrameCameraTransform = core::math::Matrix(sceneView.CameraCmp->GetViewFromWorld().GetDataPtr());
}

void TiledForwardRenderer::UpdateEnvCapture(const SceneView& sceneView)
{
	const SkyboxWorldComponent* SkyboxWorldCmp = sceneView.SceneData->GetWorldComponent<SkyboxWorldComponent>();
	if (SkyboxWorldCmp == nullptr)
	{
		core::utils::gConsole.LogInfo("TiledForwardRenderer::UpdateEnvCapture SkyboxWorldComponent not found!");
	}
	else
	{
		EnvironmentCapture.PrecalculateResourcesIBL(SkyboxWorldCmp);
	}	
}

void TiledForwardRenderer::RenderEquiCube(const SceneView& sceneView)
{
	const core::math::Matrix clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();

	EquiToCubemapShader.BindProgram();
	static const core::storage::String uClipFromModel("uClipFromModel");
	EquiToCubemapShader.SetUniform(uClipFromModel, clipFromWorld);

	// GLuint CubemapID = static_cast<const GLCubemapDeviceProxy*>(SkyboxWorldCmp->GetCubemap().GetTextureProxy(->GetResourceID());

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glBindFragDataLocation((GLuint)EquiToCubemapShader.GetProgramHandle(), (GLuint)0, "color");
	glBindFragDataLocation((GLuint)EquiToCubemapShader.GetProgramHandle(), (GLuint)1, "normal");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, EnvironmentCapture.GetCurrentHDRPanorama());
	static const core::storage::String uEquirectangularMap("uEquirectangularMap");
	EquiToCubemapShader.SetUniform(uEquirectangularMap, 0);

	// glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	glBindVertexArray(RDI->PrimitivesCube->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::RenderDepthPrePass(const SceneView& sceneView)
{
	const ScreenSize screenSize = RDI->GetScreenSize();
	GLsizei viewportWidth = (GLsizei)(sceneView.Rect.GetSize().X * screenSize.Width);
	GLsizei viewportHeight = (GLsizei)(sceneView.Rect.GetSize().Y * screenSize.Height);
	glViewport(0, 0, viewportWidth, viewportHeight);
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBOdepthMap);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	DepthShader.BindProgram();
	
	const core::math::Matrix& clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();
	
	MeshQueue drawOpaqueQueue(sceneView.OpaqueQueue);
	while(drawOpaqueQueue.GetSize() > 0)
	{
		const MeshRenderingComponent* meshCmp = drawOpaqueQueue.Pop();
		const core::math::Matrix& worldFromModel = meshCmp->GetTransform().GetWorldFromModel();
		static const core::storage::String uScreenFromModel("uScreenFromModel");
		DepthShader.SetUniform(uScreenFromModel, clipFromWorld * worldFromModel);
		
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			glBindVertexArray(subMesh->GetMeshProxy()->GetResourceID());
		
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, RDI->FallbackWhiteTexture);
		
			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::ComputeLightCulling(const SceneView& sceneView)
{
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::LightCulling Time: {}, workGroupsX: {}, workGroupsY: {}",
	// 	Time, workGroupsX, workGroupsY
	// );

	LightCullingShader.BindProgram();
	static const core::storage::String uViewFromWorld("uViewFromWorld");
	LightCullingShader.SetUniform(uViewFromWorld, sceneView.CameraCmp->GetViewFromWorld());
	static const core::storage::String uClipFromWorld("uClipFromWorld");
	LightCullingShader.SetUniform(uClipFromWorld, sceneView.CameraCmp->GetClipFromWorld());
	static const core::storage::String uClipFromView("uClipFromView");
	LightCullingShader.SetUniform(uClipFromView,  sceneView.CameraCmp->GetClipFromView());
	static const core::storage::String uScreenSizeX("uScreenSizeX");
	LightCullingShader.SetUniform(uScreenSizeX, RDI->GetScreenSize().Width);
	static const core::storage::String uScreenSizeY("uScreenSizeY");
	LightCullingShader.SetUniform(uScreenSizeY, RDI->GetScreenSize().Height);
	static const core::storage::String uLightCount("uLightCount");
	LightCullingShader.SetUniform(uLightCount, (int)std::min((int)sceneView.PointLightList.size(), MAX_NUM_LIGHTS));

	// Bind depth map texture to texture location 4 (which will not be used by any model texture)
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation((GLuint)(LightCullingShader.GetProgramHandle()), "uDepthMap"), 0);
	glBindTexture(GL_TEXTURE_2D, PreDepthBuffer);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, LightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, VisibleLightIndicesBuffer);

	glDispatchCompute(WorkGroupsX, WorkGroupsY, 1);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TiledForwardRenderer::RenderOpaqueLit(const SceneView& sceneView)
{
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::AccumulateLights");

	const ScreenSize screenSize = RDI->GetScreenSize();
	GLsizei viewportWidth = (GLsizei)(sceneView.Rect.GetSize().X * screenSize.Width);
	GLsizei viewportHeight = (GLsizei)(sceneView.Rect.GetSize().Y * screenSize.Height);
	glViewport(0, 0, viewportWidth, viewportHeight);

	const int activeTextures = 10;
	for (int i = activeTextures; i > 0; --i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// float time = (float)TimeSystem::GetTimerElapsedTime(sceneView.SceneData, eEngineTimer::GAMEPLAY);
	// LightAccumulationShader.SetUniform("uTime", time);
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::RenderOpaqueLit uTime: {}", time);

	// shadownap uniforms
	core::math::Matrix projDirLightFromWorld = sceneView.DirectionalLightList.empty()
		? core::math::Matrix()
		: GetProjectionForShadowMap(sceneView, ShadowMap.GetShadowMapResolution());

	LightAccumulationShader.BindProgram();
	static const core::storage::String uDirLightFromWorld("uDirLightFromWorld");
	LightAccumulationShader.SetUniform(uDirLightFromWorld, projDirLightFromWorld);
	static const core::storage::String uShadowType("uShadowType");
	LightAccumulationShader.SetUniform(uShadowType, (int)(ShadowMap.GetShadowType()));
	switch (ShadowMap.GetShadowType())
	{
		case eShadowType::PCF:
		{
			static const core::storage::String uDirShadowMap("uDirShadowMap");
			LightAccumulationShader.BindSampler(uDirShadowMap, 9, ShadowMap.GetDirShadowMapColor());
			static const core::storage::String uShadowBias("uShadowBias");
			LightAccumulationShader.SetUniform(uShadowBias, sceneView.SettingsCmp->PCFBias);
			break;
		}
		case eShadowType::EVSM2:
		case eShadowType::EVSM4:
		{
			static const core::storage::String uDirEVSMap("uDirEVSMap");
			LightAccumulationShader.BindSampler(uDirEVSMap, 10, ShadowMap.GetEVSMap0());
			static const core::storage::String uPositiveExponent("uPositiveExponent");
			LightAccumulationShader.SetUniform(uPositiveExponent, sceneView.SettingsCmp->EVSMPositiveExponent);
			static const core::storage::String uNegativeExponent("uNegativeExponent");
			LightAccumulationShader.SetUniform(uNegativeExponent, sceneView.SettingsCmp->EVSMNegativeExponent);
			static const core::storage::String uEVSMBias("uEVSMBias");
			LightAccumulationShader.SetUniform(uEVSMBias, sceneView.SettingsCmp->EVSMBias);
			static const core::storage::String uLightBleedingReduction("uLightBleedingReduction");
			LightAccumulationShader.SetUniform(uLightBleedingReduction, sceneView.SettingsCmp->EVSMLghtBleedingReduction);
			break;
		}
		default:
			break;
	}

	// Lighting uniforms
	const EntityTransform& cameraTransform = sceneView.CameraCmp->GetTransform();
	static const core::storage::String uLightCount("uLightCount");
	LightAccumulationShader.SetUniform(uLightCount, (int)std::min((int)sceneView.PointLightList.size(), MAX_NUM_LIGHTS));
	static const core::storage::String uWorkGroupsX("uWorkGroupsX");
	LightAccumulationShader.SetUniform(uWorkGroupsX, (int)WorkGroupsX);
	static const core::storage::String uViewPosition("uViewPosition");
	LightAccumulationShader.SetUniform(uViewPosition, cameraTransform.GetGlobalTranslation());

	int dirLightsCount = 0;
	for (const DirectionalLightComponent* dirLightCmp : sceneView.DirectionalLightList)
	{		
		const EntityTransform& transform = dirLightCmp->GetTransform();
		core::storage::StringBuilder sb;
		sb.AppendFormat("uDirectionalLight[{}].", dirLightsCount);
		core::storage::String baseName = sb.StealString();
		core::math::Color colorIntensity = dirLightCmp->GetColor();
		colorIntensity.A = dirLightCmp->GetIntensity();
		LightAccumulationShader.SetUniform(baseName + "ColorIntensity", colorIntensity);
		LightAccumulationShader.SetUniform(baseName + "Direction", -transform.GetGlobalForward());

		++dirLightsCount;
		if (dirLightsCount == MAX_LIGHT_COUNT_DIRECTIONAL)
			break;
	}
	static const core::storage::String uDirectionalLightCount("uDirectionalLightCount");
	LightAccumulationShader.SetUniform(uDirectionalLightCount, dirLightsCount);
	
	static const core::storage::String uIrradianceMap("uIrradianceMap");
	LightAccumulationShader.BindSamplerCube(uIrradianceMap, 0, EnvironmentCapture.GetCurrentIrradianceMap());
	static const core::storage::String uPrefilterMap("uPrefilterMap");
	LightAccumulationShader.BindSamplerCube(uPrefilterMap, 1, EnvironmentCapture.GetCurrentPrefilterMap());
	static const core::storage::String uBrdfLUT("uBrdfLUT");
	LightAccumulationShader.BindSampler(uBrdfLUT, 2, PreintegratedBrdfLUT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, LightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, VisibleLightIndicesBuffer);
	
	glBindFragDataLocation((GLuint)LightAccumulationShader.GetProgramHandle(), 0, "oColor");
	glBindFragDataLocation((GLuint)LightAccumulationShader.GetProgramHandle(), 1, "oNormal");

	const core::math::Matrix& clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();
	
	core::storage::PriorityQueue<const MeshRenderingComponent*, SceneView::DistanceToCameraComparator> drawOpaqueQueue(sceneView.OpaqueQueue);
	while (drawOpaqueQueue.GetSize() > 0)
	{
		const MeshRenderingComponent* meshCmp = drawOpaqueQueue.Pop();
		const SkeletalAnimationComponent* animCmp = meshCmp->GetSibling<SkeletalAnimationComponent>();

		const EntityTransform& transform = meshCmp->GetTransform();
		const core::math::Matrix& worldFromModel = transform.GetWorldFromModel();
		static const core::storage::String uClipFromModel("uClipFromModel");
		LightAccumulationShader.SetUniform(uClipFromModel, clipFromWorld * worldFromModel);
		static const core::storage::String uWorldFromModel("uWorldFromModel");
		LightAccumulationShader.SetUniform(uWorldFromModel, worldFromModel);

		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			static const core::storage::String uHasBones("uHasBones");
			LightAccumulationShader.SetUniform(uHasBones, (subMesh->GetBones().size() != 0) ? 1.0f : 0.0f );

			 for (int i = 0; i < 64; ++i)
			 {
				core::storage::StringBuilder sb;
				sb.AppendFormat("uBones[{}]", i);
			 	LightAccumulationShader.SetUniform(sb.GetString(), core::math::Matrix());
			 }
			 
			 if (animCmp)
			 {
			 	int count = 0;
			 	for (auto& b : subMesh->GetBones())
			 	{
					auto it = animCmp->ModelFromBone.find(b.name);
					if (it == animCmp->ModelFromBone.end())
						continue;

			 		core::math::Matrix animFromModel = it->second * b.boneFromModel;
					core::storage::StringBuilder sb;
					sb.AppendFormat("uBones[{}]", count);
			 		LightAccumulationShader.SetUniform(sb.GetString(), animFromModel);
					// LightAccumulationShader.SetUniform(baseName, core::math::Matrix::IDENTITY);
			 		if (count >= 64) break;
			 		count++;
			 	}
			 }

			Material material = meshCmp->GetMaterial(i);
			static const core::storage::String uMaterialEmissive("uMaterial.Emissive");
			LightAccumulationShader.SetUniform(uMaterialEmissive, material.Emissive);
			static const core::storage::String uMaterialAlbedo("uMaterial.Albedo");
			LightAccumulationShader.SetUniform(uMaterialAlbedo, material.Albedo);
			static const core::storage::String uMaterialRoughness("uMaterial.Roughness");
			LightAccumulationShader.SetUniform(uMaterialRoughness, material.Roughness);
			static const core::storage::String uMaterialMetallic("uMaterial.Metallic");
			LightAccumulationShader.SetUniform(uMaterialMetallic, material.Metallic);
			static const core::storage::String uMaterialOpacityMaskThreshold("uMaterial.OpacityMaskThreshold");
			LightAccumulationShader.SetUniform(uMaterialOpacityMaskThreshold, material.OpacityMaskThreshold);

			const TextureResource* emissiveMap = subMesh->GetMeshData().GetEmissiveMap();
			const TextureResource* albedoMap = subMesh->GetMeshData().GetAlbedoMap();
			const TextureResource* roughnessMap = subMesh->GetMeshData().GetRoughnessMap();
			const TextureResource* metallicMap = subMesh->GetMeshData().GetMetallicMap();
			const TextureResource* normalMap = subMesh->GetMeshData().GetNormalMap();
			const TextureResource* ambientOcclusionMap = subMesh->GetMeshData().GetAmbientOcclusionMap();

			// Material textures
			static const core::storage::String uEmissiveMap("uEmissiveMap");
			LightAccumulationShader.BindSampler(uEmissiveMap,			3, emissiveMap			? emissiveMap->GetTextureProxy()->GetResourceID()			: RDI->FallbackBlackTexture);
			static const core::storage::String uAlbedoMap("uAlbedoMap");
			LightAccumulationShader.BindSampler(uAlbedoMap,			4, albedoMap			? albedoMap->GetTextureProxy()->GetResourceID()				: RDI->FallbackWhiteTexture);
			static const core::storage::String uRoughnessMap("uRoughnessMap");
			LightAccumulationShader.BindSampler(uRoughnessMap,		5, roughnessMap			? roughnessMap->GetTextureProxy()->GetResourceID()			: RDI->FallbackWhiteTexture);
			static const core::storage::String uMetallicMap("uMetallicMap");
			LightAccumulationShader.BindSampler(uMetallicMap,			6, metallicMap			? metallicMap->GetTextureProxy()->GetResourceID()			: RDI->FallbackWhiteTexture);
			static const core::storage::String uNormalMap("uNormalMap");
			LightAccumulationShader.BindSampler(uNormalMap,			7, normalMap			? normalMap->GetTextureProxy()->GetResourceID()				: RDI->FallbackNormalMap);
			static const core::storage::String uAmbientOcclusionMap("uAmbientOcclusionMap");
			LightAccumulationShader.BindSampler(uAmbientOcclusionMap,	8, ambientOcclusionMap	? ambientOcclusionMap->GetTextureProxy()->GetResourceID()	: RDI->FallbackWhiteTexture);

			const GLuint subMeshVAO = (GLuint)(subMesh->GetMeshProxy()->GetResourceID());
			glBindVertexArray(subMeshVAO);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			++i;
		}
	}
	
	// CHECK_GL_ERR();

	// Clear bound resources
	glBindVertexArray(0);

	for (int i = activeTextures; i > 0; --i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// CHECK_GL_ERR();
}

void TiledForwardRenderer::RenderSkybox(const SceneView& sceneView)
{
	if (EnvironmentCapture.GetCurrentEnvCubemap() > 0)
	{
		core::math::Color tint = core::math::Color::WHITE;
		SkyboxWorldComponent* skyboxCmp = sceneView.SceneData->GetWorldComponent<SkyboxWorldComponent>();
		if (skyboxCmp)
		{
			tint = skyboxCmp->GetTint();
			EnvironmentCapture.SetCurrentEnviroment(skyboxCmp->GetCurrentIndex());
		}

		const core::math::Matrix clipFromView = sceneView.CameraCmp->GetClipFromView();
		core::math::Matrix viewFromWorld = sceneView.CameraCmp->GetViewFromWorld();
		// center cube in view space by setting translation to 0 for x, y and z.
		// SetTranslation resets core::math::Matrix to identity
		viewFromWorld.Data[3] = 0.0f;
		viewFromWorld.Data[7] = 0.0f;
		viewFromWorld.Data[11] = 0.0f;

		core::math::Matrix clipFromWorld = clipFromView * viewFromWorld;

		glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);
		
		SkyboxShader.BindProgram();
		static const core::storage::String uTint("uTint");
		SkyboxShader.SetUniform(uTint, tint);
		static const core::storage::String uClipFromWorld("uClipFromWorld");
		SkyboxShader.SetUniform(uClipFromWorld, clipFromWorld);
		static const core::storage::String uCubemap("uCubemap");
		SkyboxShader.BindSamplerCube(uCubemap, 0, EnvironmentCapture.GetCurrentEnvCubemap());
		
		glBindFragDataLocation((GLuint)LightAccumulationShader.GetProgramHandle(), 0, "color");
		glBindFragDataLocation((GLuint)LightAccumulationShader.GetProgramHandle(), 1, "normal");
		
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);

		glBindVertexArray(RDI->PrimitivesCube->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void TiledForwardRenderer::RenderTranslucentLit(const SceneView& sceneView)
{
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::RenderTranslucenLit");
	const int activeTextures = 8;
	for (int i = activeTextures; i > 0; --i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	const EntityTransform& cameraTransform = sceneView.CameraCmp->GetTransform();
	TranslucentShader.BindProgram();
	static const core::storage::String uViewPosition("uViewPosition");
	TranslucentShader.SetUniform(uViewPosition, cameraTransform.GetGlobalTranslation());

	int dirLightsCount = 0;
	for (const DirectionalLightComponent* dirLightCmp : sceneView.DirectionalLightList)
	{
		const EntityTransform& transform = dirLightCmp->GetTransform();
		core::storage::StringBuilder sb;
		sb.AppendFormat("uDirectionalLight[{}].", dirLightsCount);

		core::storage::String baseName = sb.StealString();
		core::math::Color colorIntensity = dirLightCmp->GetColor();
		colorIntensity.A = dirLightCmp->GetIntensity();
		TranslucentShader.SetUniform(baseName + "ColorIntensity", colorIntensity);
		TranslucentShader.SetUniform(baseName + "Direction", -(transform.GetGlobalForward()));

		++dirLightsCount;
		if (dirLightsCount == MAX_LIGHT_COUNT_DIRECTIONAL)
			break;
	}
	static const core::storage::String uDirectionalLightCount("uDirectionalLightCount");
	TranslucentShader.SetUniform(uDirectionalLightCount, dirLightsCount);

	static const core::storage::String uIrradianceMap("uIrradianceMap");
	TranslucentShader.BindSamplerCube(uIrradianceMap, 0, EnvironmentCapture.GetCurrentIrradianceMap());
	static const core::storage::String uPrefilterMap("uPrefilterMap");
	TranslucentShader.BindSamplerCube(uPrefilterMap, 1, EnvironmentCapture.GetCurrentPrefilterMap());
	static const core::storage::String uBrdfLUT("uBrdfLUT");
	TranslucentShader.BindSampler(uBrdfLUT, 2, PreintegratedBrdfLUT);

	glBindFragDataLocation((GLuint)TranslucentShader.GetProgramHandle(), 0, "oColor");
	glBindFragDataLocation((GLuint)TranslucentShader.GetProgramHandle(), 1, "oNormal");

	const core::math::Matrix& clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();
	// for (const MeshRenderingComponent* meshCmp : sceneView.TranslucentQueue)
	// {
	core::storage::PriorityQueue<const MeshRenderingComponent*, SceneView::DistanceToCameraComparator> drawTranslucentQueue(sceneView.TranslucentQueue);
	while (drawTranslucentQueue.GetSize() > 0)
	{
		const MeshRenderingComponent* meshCmp = drawTranslucentQueue.Pop();
		const EntityTransform& transform = meshCmp->GetTransform();
		const core::math::Matrix& worldFromModel = transform.GetWorldFromModel();
		static const core::storage::String uClipFromModel("uClipFromModel");
		TranslucentShader.SetUniform(uClipFromModel, clipFromWorld * worldFromModel);
		static const core::storage::String uWorldFromModel("uWorldFromModel");
		TranslucentShader.SetUniform(uWorldFromModel, worldFromModel);

		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			Material material = meshCmp->GetMaterial(i);
			static const core::storage::String uMaterialEmissive("uMaterial.Emissive");
			TranslucentShader.SetUniform(uMaterialEmissive, material.Emissive);
			static const core::storage::String uMaterialAlbedo("uMaterial.Albedo");
			TranslucentShader.SetUniform(uMaterialAlbedo, material.Albedo);
			static const core::storage::String uMaterialRoughness("uMaterial.Roughness");
			TranslucentShader.SetUniform(uMaterialRoughness, material.Roughness);
			static const core::storage::String uMaterialMetallic("uMaterial.Metallic");
			TranslucentShader.SetUniform(uMaterialMetallic, material.Metallic);
			static const core::storage::String uMaterialOpacityMaskThreshold("uMaterial.OpacityMaskThreshold");
			TranslucentShader.SetUniform(uMaterialOpacityMaskThreshold, material.OpacityMaskThreshold);


			const TextureResource* emissiveMap = subMesh->GetMeshData().GetEmissiveMap();
			const TextureResource* albedoMap = subMesh->GetMeshData().GetAlbedoMap();
			const TextureResource* roughnessMap = subMesh->GetMeshData().GetRoughnessMap();
			const TextureResource* metallicMap = subMesh->GetMeshData().GetMetallicMap();
			const TextureResource* normalMap = subMesh->GetMeshData().GetNormalMap();
			const TextureResource* ambientOcclusionMap = subMesh->GetMeshData().GetAmbientOcclusionMap();

			// Material textures
			static const core::storage::String uEmissiveMap("uEmissiveMap");
			TranslucentShader.BindSampler(uEmissiveMap, 3, emissiveMap ? emissiveMap->GetTextureProxy()->GetResourceID() : RDI->FallbackBlackTexture);
			static const core::storage::String uAlbedoMap("uAlbedoMap");
			TranslucentShader.BindSampler(uAlbedoMap, 4, albedoMap ? albedoMap->GetTextureProxy()->GetResourceID() : RDI->FallbackWhiteTexture);
			static const core::storage::String uRoughnessMap("uRoughnessMap");
			TranslucentShader.BindSampler(uRoughnessMap, 5, roughnessMap ? roughnessMap->GetTextureProxy()->GetResourceID() : RDI->FallbackWhiteTexture);
			static const core::storage::String uMetallicMap("uMetallicMap");
			TranslucentShader.BindSampler(uMetallicMap, 6, metallicMap ? metallicMap->GetTextureProxy()->GetResourceID() : RDI->FallbackWhiteTexture);
			static const core::storage::String uNormalMap("uNormalMap");
			TranslucentShader.BindSampler(uNormalMap, 7, normalMap ? normalMap->GetTextureProxy()->GetResourceID() : RDI->FallbackNormalMap);
			static const core::storage::String uAmbientOcclusionMap("uAmbientOcclusionMap");
			TranslucentShader.BindSampler(uAmbientOcclusionMap, 8, ambientOcclusionMap ? ambientOcclusionMap->GetTextureProxy()->GetResourceID() : RDI->FallbackWhiteTexture);

			const GLuint subMeshVAO = (GLuint)(subMesh->GetMeshProxy()->GetResourceID());
			glBindVertexArray(subMeshVAO);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			++i;
		}
	}
	
	// CHECK_GL_ERR();

	// Clear bound resources
	glBindVertexArray(0);

	for (int i = activeTextures; i > 0; --i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// CHECK_GL_ERR();
}

void TiledForwardRenderer::RenderParticleUnlit(const SceneView& sceneView)
{
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::RenderParticleUnlit");

	float time = (float)TimeSystem::GetTimerElapsedTime(sceneView.SceneData, eEngineTimer::GAMEPLAY);
	const core::math::Matrix& viewFromWorld = sceneView.CameraCmp->GetViewFromWorld();
	const core::math::Matrix& screenFromView = sceneView.CameraCmp->GetClipFromView();

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	const ScreenSize screenSize = RDI->GetScreenSize();
	float viewportWidth = sceneView.Rect.GetSize().X * screenSize.Width;
	float viewportHeight = sceneView.Rect.GetSize().Y * screenSize.Height;

	ParticleShader.BindProgram();

	static const core::storage::String uTime("uTime");
	ParticleShader.SetUniform(uTime, time);
	static const core::storage::String uScreenSize("uScreenSize");
	ParticleShader.SetUniform(uScreenSize, core::math::Vector(viewportWidth, viewportHeight, 1.0f / viewportWidth, 1.0f / viewportHeight));
	static const core::storage::String uNear("uNear");
	ParticleShader.SetUniform(uNear, sceneView.CameraCmp->GetClippingPlaneNear());
	static const core::storage::String uFar("uFar");
	ParticleShader.SetUniform(uFar, sceneView.CameraCmp->GetClippingPlaneFar());
	static const core::storage::String uScreenFromView("uScreenFromView");
	ParticleShader.SetUniform(uScreenFromView, screenFromView);

	glBindFragDataLocation((GLuint)TranslucentShader.GetProgramHandle(), 0, "color");
	glBindFragDataLocation((GLuint)TranslucentShader.GetProgramHandle(), 1, "normal");

	for (auto [particleCmp] : sceneView.SceneData->IterateComponents<ParticleComponent>())
	{
		const EntityTransform& transform = particleCmp->GetTransform();
		const core::math::Matrix& worldFromModel = particleCmp->GetEmitter()->GetSettings().SimulationSpace == ParticleEmitter::eSimulationSpace::LOCAL_SPACE
			? transform.GetWorldFromModel()
			: core::math::Matrix();

		static const core::storage::String uViewFromWorld("uViewFromWorld");
		ParticleShader.SetUniform(uViewFromWorld, viewFromWorld);
		static const core::storage::String uWorldFromModel("uWorldFromModel");
		ParticleShader.SetUniform(uWorldFromModel, worldFromModel);

		ParticleEmitter::Settings emitterSettings = particleCmp->GetEmitter()->GetSettings();
		static const core::storage::String uEmitterAlbedo("uEmitterAlbedo");
		ParticleShader.SetUniform(uEmitterAlbedo, emitterSettings.Albedo);
		static const core::storage::String uEmitterEmissive("uEmitterEmissive");
		ParticleShader.SetUniform(uEmitterEmissive, emitterSettings.Emissive);

		SpritesheetSettings spriteSettings = emitterSettings.Spritesheet;
		static const core::storage::String uSpriteColor("uSpriteColor");
		ParticleShader.SetUniform(uSpriteColor, spriteSettings.SpriteColor);
		static const core::storage::String uSpriteDepthFade("uSpriteDepthFade");
		ParticleShader.SetUniform(uSpriteDepthFade, spriteSettings.SpriteDepthFade);
		float startFrame = spriteSettings.IsRandomStartFrame
			? core::math::RandomRange(0.0f, spriteSettings.SubImages.X * spriteSettings.SubImages.Y)
			: spriteSettings.StartFrame;
		static const core::storage::String uSpriteStartFrame("uSpriteStartFrame");
		ParticleShader.SetUniform(uSpriteStartFrame, startFrame);
		static const core::storage::String uSpriteSpeed("uSpriteSpeed");
		ParticleShader.SetUniform(uSpriteSpeed, spriteSettings.Speed);
		static const core::storage::String uSpriteSubImages("uSpriteSubImages");
		ParticleShader.SetUniform(uSpriteSubImages, spriteSettings.SubImages.X, spriteSettings.SubImages.Y);

		GLsizei partileLen = (GLsizei)(particleCmp->GetEmitter()->GetInstancesCount());
		const TextureResource* texture = particleCmp->GetEmitter()->GetSpritesheet();
		const GLParticleDeviceProxy* particleProxy = static_cast<const GLParticleDeviceProxy*>(particleCmp->GetEmitter()->GetParticleProxy());
		GLuint particleVAO = particleProxy->GetVAO();

		GLuint textureID = texture == nullptr
			? RDI->FallbackWhiteTexture
			: (GLuint)(texture->GetTextureProxy()->GetResourceID());

		static const core::storage::String uHasSprite("uHasSprite");
		ParticleShader.SetUniform(uHasSprite, texture == nullptr ? 0.0f : 1.0f);

		static const core::storage::String uSpriteMap("uSpriteMap");
		ParticleShader.BindSampler(uSpriteMap, 0, textureID);
		static const core::storage::String uLinearDepth("uLinearDepth");
		ParticleShader.BindSampler(uLinearDepth, 1, LinearDepth);

		glBindVertexArray(particleVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, partileLen);
		glBindVertexArray(0);
		// CHECK_GL_ERR();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostLinearizeDepth(const SceneView& sceneView)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const ScreenSize screenSize = RDI->GetScreenSize();
	GLsizei viewportWidth = (GLsizei)(sceneView.Rect.GetSize().X * screenSize.Width);
	GLsizei viewportHeight = (GLsizei)(sceneView.Rect.GetSize().Y * screenSize.Height);
	glViewport(0, 0, viewportWidth, viewportHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LinearDepth, 0);

	LinearizeDepthShader.BindProgram();
	static const core::storage::String uDepth("uDepth");
	LinearizeDepthShader.BindSampler(uDepth, 0, PreDepthBuffer);
	static const core::storage::String uNear("uNear");
	LinearizeDepthShader.SetUniform(uNear, sceneView.CameraCmp->GetClippingPlaneNear());
	static const core::storage::String uFar("uFar");
	LinearizeDepthShader.SetUniform(uFar, sceneView.CameraCmp->GetClippingPlaneFar());

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostColorBuffer1, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostMotionBlur(const SceneView& sceneView)
{
	float motionBlurScale = 0.0f;
	const PostprocessSettingsComponent* postCmp = sceneView.CameraCmp->GetSibling<PostprocessSettingsComponent>();
	if (postCmp) {
		motionBlurScale = postCmp->MotionBlurScale;
	}

	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(sceneView.SceneData, Poly::eEngineTimer::GAMEPLAY));
	float currentFPS = 1.0f / deltaTime;
	float targetFPS = 60.0f;

	float maxDiff = 0.0f;
	float maxDiffThreshold = 0.001f;
	const float* matrixArray = sceneView.CameraCmp->GetViewFromWorld().GetDataPtr();
	const float* prevMatrixArray = PreviousFrameCameraTransform.GetDataPtr();
	for(size_t i = 0; i < 16; ++i)
	{
		maxDiff = std::max(maxDiff, std::abs(matrixArray[i] - prevMatrixArray[i]));
	}

	float jitterFix = maxDiff > maxDiffThreshold ? 1.0f : 0.0f;
	 
	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);

	const ScreenSize screenSize = RDI->GetScreenSize();
	GLsizei viewportWidth = (GLsizei)(sceneView.Rect.GetSize().X * screenSize.Width);
	GLsizei viewportHeight = (GLsizei)(sceneView.Rect.GetSize().Y * screenSize.Height);
	glViewport(0, 0, viewportWidth, viewportHeight);
	
	MotionBlurShader.BindProgram();
	static const core::storage::String uImage("uImage");
	MotionBlurShader.BindSampler(uImage, 0, ColorBuffer);
	static const core::storage::String uDepth("uDepth");
	MotionBlurShader.BindSampler(uDepth, 1, LinearDepth);
	static const core::storage::String uPrevClipFromWorld("uPrevClipFromWorld");
	MotionBlurShader.SetUniform(uPrevClipFromWorld, PreviousFrameCameraClipFromWorld);
	static const core::storage::String uWorldFromView("uWorldFromView");
	MotionBlurShader.SetUniform(uWorldFromView, sceneView.CameraCmp->GetViewFromWorld().GetInversed());
	static const core::storage::String uScale("uScale");
	MotionBlurShader.SetUniform(uScale, (currentFPS / targetFPS) * motionBlurScale * jitterFix);
	 
	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostDepthOfField(const SceneView& sceneView)
{
	float dofPoint = 1000.0f;
	float dofRange = 800.0f;
	float dofSize = 0.0f;
	float dofShow = 0.0f;

	const PostprocessSettingsComponent* postCmp = sceneView.CameraCmp->GetSibling<PostprocessSettingsComponent>();
	if (postCmp)
	{
		dofPoint = postCmp->DOFPoint;
		dofRange = postCmp->DOFRange;
		dofSize = postCmp->DOFSize;
		dofShow = postCmp->DOFShow;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostColorBufferHalfRes, 0);

	const ScreenSize screenSize = RDI->GetScreenSize();
	GLsizei viewportWidth = (GLsizei)(screenSize.Width * sceneView.Rect.GetSize().X);
	GLsizei viewportHeight = (GLsizei)(screenSize.Height * sceneView.Rect.GetSize().Y);
	glViewport(0, 0, (GLsizei)(0.5f * viewportWidth), (GLsizei)(0.5f * viewportHeight));

	DOFBokehShader.BindProgram();
	static const core::storage::String uRes("uRes");
	DOFBokehShader.SetUniform(uRes, core::math::Vector(0.5f * viewportWidth, 0.5f * viewportHeight, 2.0f / viewportWidth, 2.0f / viewportHeight));
	static const core::storage::String uDOFPoint("uDOFPoint");
	DOFBokehShader.SetUniform(uDOFPoint, dofPoint);
	static const core::storage::String uDOFRange("uDOFRange");
	DOFBokehShader.SetUniform(uDOFRange, dofRange);
	static const core::storage::String uDOFSize("uDOFSize");
	DOFBokehShader.SetUniform(uDOFSize, dofSize);
	static const core::storage::String uImage("uImage");
	DOFBokehShader.BindSampler(uImage, 0, PostColorBuffer1);
	static const core::storage::String uDepth("uDepth");
	DOFBokehShader.BindSampler(uDepth, 1, LinearDepth);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	glViewport(0, 0, viewportWidth, viewportHeight);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostColorBuffer0, 0);

	DOFApplyShader.BindProgram();
	DOFApplyShader.BindSampler(uImage, 0, PostColorBuffer1);
	static const core::storage::String uDOF("uDOF");
	DOFApplyShader.BindSampler(uDOF, 1, PostColorBufferHalfRes);
	DOFApplyShader.BindSampler(uDepth, 2, LinearDepth);
	DOFApplyShader.SetUniform(uDOFPoint, dofPoint);
	DOFApplyShader.SetUniform(uDOFRange, dofRange);
	static const core::storage::String uDOFShow("uDOFShow");
	DOFApplyShader.SetUniform(uDOFShow, dofShow);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostBloom(const SceneView& sceneView)
{
	float time = (float)TimeSystem::GetTimerElapsedTime(sceneView.SceneData, eEngineTimer::GAMEPLAY);

	float bloomThreshold = 1.0f;
	float bloomScale = 0.1f;
	float bloomBlurScaleX = 1.0f;
	float bloomBlurScaleY = 1.0f;
	const PostprocessSettingsComponent* postCmp = sceneView.CameraCmp->GetSibling<PostprocessSettingsComponent>();
	if (postCmp) {
		bloomThreshold	= postCmp->BloomThreshold;
		bloomScale		= postCmp->BloomScale;
		bloomBlurScaleX = postCmp->BloomBlurScaleX;
		bloomBlurScaleY = postCmp->BloomBlurScaleY;
	}

	const ScreenSize screenSize = RDI->GetScreenSize();
	GLsizei viewportWidth = (GLsizei)(screenSize.Width * sceneView.Rect.GetSize().X);
	GLsizei viewportHeight = (GLsizei)(screenSize.Height * sceneView.Rect.GetSize().Y);
	glViewport(0, 0, (GLsizei)(0.5f * viewportWidth), (GLsizei)(0.5f * viewportHeight));

	glBindFramebuffer(GL_FRAMEBUFFER, RTBloom.GetWriteFBO());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RTBloom.GetWriteTarget(), 0);

	BloomBrightShader.BindProgram();
	static const core::storage::String uBrightThreshold("uBrightThreshold");
	BloomBrightShader.SetUniform(uBrightThreshold, bloomThreshold);
	static const core::storage::String uImage("uImage");
	BloomBrightShader.BindSampler(uImage, 0, PostColorBuffer0);
	
	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	RTBloom.Flip();
	
	BloomBlurShader.BindProgram();
	for (size_t i = 0; i < 6; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, RTBloom.GetWriteFBO());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RTBloom.GetWriteTarget(), 0);

		static const core::storage::String uImage("uImage");
		BloomBlurShader.BindSampler(uImage, 0, RTBloom.GetReadTarget());
		static const core::storage::String uIsHorizontal("uIsHorizontal");
		BloomBlurShader.SetUniform(uIsHorizontal, (i % 2 > 0) ? 1.0f : 0.0f);
		static const core::storage::String uBlurScale("uBlurScale");
		BloomBlurShader.SetUniform(uBlurScale, (i % 2 > 0) ? bloomBlurScaleX : bloomBlurScaleY);
		static const core::storage::String uTime("uTime");
		BloomBlurShader.SetUniform(uTime, time);

		glBindVertexArray(RDI->PrimitivesQuad->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		RTBloom.Flip();
	}
	
	glViewport(0, 0, viewportWidth, viewportHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostColorBuffer1, 0);

	BloomApplyShader.BindProgram();
	BloomApplyShader.BindSampler(uImage, 0, PostColorBuffer0);
	static const core::storage::String uBloom("uBloom");
	BloomApplyShader.BindSampler(uBloom, 1, RTBloom.GetReadTarget());
	static const core::storage::String uBloomScale("uBloomScale");
	BloomApplyShader.SetUniform(uBloomScale, bloomScale);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostTonemapper(const SceneView& sceneView)
{
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::PostTonemapper");

	float exposure = 1.0f;
	const PostprocessSettingsComponent* postCmp = sceneView.CameraCmp->GetSibling<PostprocessSettingsComponent>();
	if (postCmp) {
		exposure = postCmp->Exposure;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost0);

	HDRShader.BindProgram();
	static const core::storage::String uHdrBuffer("uHdrBuffer");
	HDRShader.BindSampler(uHdrBuffer, 0, PostColorBuffer1);
	static const core::storage::String uExposure("uExposure");
	HDRShader.SetUniform(uExposure, exposure);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostFXAA(const SceneView& sceneView)
{
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::PostFXAA");

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);

	FXAAShader.BindProgram();
	static const core::storage::String uImage("uImage");
	FXAAShader.BindSampler(uImage, 0, PostColorBuffer0);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostGamma(const SceneView& sceneView)
{
	float time = (float)TimeSystem::GetTimerElapsedTime(sceneView.SceneData, eEngineTimer::GAMEPLAY);

	float planeNear = sceneView.CameraCmp->GetClippingPlaneNear();
	float planeFar = sceneView.CameraCmp->GetClippingPlaneFar();

	const ScreenSize screenSize = RDI->GetScreenSize();
	float viewportWidth = sceneView.Rect.GetSize().X * screenSize.Width;
	float viewportHeight = sceneView.Rect.GetSize().Y * screenSize.Height;

	float grainScale = 0.01f;
	float vignetteScale = 0.1f;
	float abberationScale = 0.1f;
	core::math::Color tint = core::math::Color::WHITE;
	core::math::Color fogColor = core::math::Color::WHITE;
	float fogDensity = 0.66f;
	// float temperature = 6500.0;
	float gamma = 2.2f;

	const PostprocessSettingsComponent* postCmp = sceneView.CameraCmp->GetSibling<PostprocessSettingsComponent>();
	if (postCmp) {
		grainScale		= postCmp->GrainScale;
		vignetteScale	= postCmp->VignetteScale;
		abberationScale	= postCmp->AbberationScale;
		tint			= postCmp->Tint;
		fogColor		= postCmp->FogColor;
		fogDensity		= postCmp->FogDensity;
		// temperature		= postCmp->Temperature;
		gamma			= postCmp->Gamma;
	}

	GammaShader.BindProgram();
	static const core::storage::String uImage("uImage");
	GammaShader.BindSampler(uImage, 0, PostColorBuffer1);
	static const core::storage::String uDepth("uDepth");
	GammaShader.BindSampler(uDepth, 1, LinearDepth);
	static const core::storage::String uSplashImage("uSplashImage");
	GammaShader.BindSampler(uSplashImage, 2, Splash->GetTextureProxy()->GetResourceID());
	static const core::storage::String uSplashTint("uSplashTint");
	GammaShader.SetUniform(uSplashTint, core::math::Color(1.0f, 1.0f, 1.0f, 1.0f) * 0.7f);
	static const core::storage::String uTime("uTime");
	GammaShader.SetUniform(uTime, time);
	static const core::storage::String uRes("uRes");
	GammaShader.SetUniform(uRes, core::math::Vector(viewportWidth, viewportHeight, 1.0f / viewportWidth, 1.0f / viewportHeight));
	static const core::storage::String uTint("uTint");
	GammaShader.SetUniform(uTint, tint);
	static const core::storage::String uFogColor("uFogColor");
	GammaShader.SetUniform(uFogColor, fogColor);
	static const core::storage::String uFogDensity("uFogDensity");
	GammaShader.SetUniform(uFogDensity, fogDensity);
	static const core::storage::String uPlaneFar("uPlaneFar");
	GammaShader.SetUniform(uPlaneFar, planeFar);
	static const core::storage::String uPlaneNear("uPlaneNear");
	GammaShader.SetUniform(uPlaneNear, planeNear);
	static const core::storage::String uGrainScale("uGrainScale");
	GammaShader.SetUniform(uGrainScale, grainScale);
	static const core::storage::String uVignetteScale("uVignetteScale");
	GammaShader.SetUniform(uVignetteScale, vignetteScale);
	static const core::storage::String uAbberationScale("uAbberationScale");
	GammaShader.SetUniform(uAbberationScale, abberationScale);
	// GammaShader.SetUniform("uTemperature", temperature);
	static const core::storage::String uGamma("uGamma");
	GammaShader.SetUniform(uGamma, gamma);

	glViewport(
		(GLint)(sceneView.Rect.GetMin().X * screenSize.Width),
		(GLint)(sceneView.Rect.GetMin().Y * screenSize.Height),
		(GLsizei)viewportWidth,
		(GLsizei)viewportHeight
	);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void TiledForwardRenderer::EditorDebug(const SceneView& sceneView)
{
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::EditorDebug");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	ScreenSize screenSize = RDI->GetScreenSize();
	glViewport(0, 0, screenSize.Width, screenSize.Height);
	
	glDisable(GL_DEPTH_TEST);

	const core::math::Matrix& clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();
	EditorDebugShader.BindProgram();
	static const core::storage::String uMVP("uMVP");
	EditorDebugShader.SetUniform(uMVP, clipFromWorld);

	// Render Lines
	{
		auto debugLinesComponent = sceneView.SceneData->GetWorldComponent<DebugDrawStateWorldComponent>();
		auto& debugLines = debugLinesComponent->DebugLines;
		auto& debugLinesColors = debugLinesComponent->DebugLinesColors;

		DebugRenderingBuffers debugLinesBuffers; // vertex and color buffer

												 // set up buffer
		glBindVertexArray(debugLinesBuffers.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, debugLinesBuffers.VBO);
		glBufferData(GL_ARRAY_BUFFER, debugLines.size() * sizeof(DebugDrawStateWorldComponent::DebugLine)
			+ debugLinesColors.size() * sizeof(DebugDrawStateWorldComponent::DebugLineColor), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, debugLines.size() * sizeof(DebugDrawStateWorldComponent::DebugLine), (GLvoid*)debugLines.data());
		glBufferSubData(GL_ARRAY_BUFFER, debugLines.size() * sizeof(DebugDrawStateWorldComponent::DebugLine), debugLinesColors.size() * sizeof(DebugDrawStateWorldComponent::DebugLineColor), (GLvoid*)debugLinesColors.data());
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(core::math::Vector3f), NULL);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(core::math::Color), (GLvoid*)(debugLines.size() * sizeof(DebugDrawStateWorldComponent::DebugLine)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, (GLsizei)debugLines.size() * 2);
		glBindVertexArray(0);

		debugLines.clear();
		debugLinesColors.clear();
	}

	glEnable(GL_DEPTH_TEST);
}

void TiledForwardRenderer::UIText2D(const SceneView& sceneView)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ScreenSize screen = RDI->GetScreenSize();
	core::math::Matrix ortho;
	ortho.SetOrthographic(
		sceneView.Rect.GetMin().Y * screen.Height,
		sceneView.Rect.GetMax().Y * screen.Height,
		sceneView.Rect.GetMin().X * screen.Width,
		sceneView.Rect.GetMax().X * screen.Width,
		-1,
		 1
	);
	Text2DShader.BindProgram();
	static const core::storage::String u_projection("u_projection");
	Text2DShader.SetUniform(u_projection, ortho);

	for (const auto& [textCmp] : sceneView.SceneData->IterateComponents<ScreenSpaceTextComponent>())
	{
		Text2D& text = textCmp->GetText();
		static const core::storage::String u_textColor("u_textColor");
		Text2DShader.SetUniform(u_textColor, text.GetFontColor());
		static const core::storage::String u_position("u_position");
		Text2DShader.SetUniform(u_position, core::math::Vector((float)textCmp->GetScreenPosition().X, (float)textCmp->GetScreenPosition().Y, 0));
		text.UpdateDeviceBuffers();

		GLuint textVAO = (GLuint)(text.GetTextFieldBuffer()->GetResourceID());
		GLuint textBufferSize = (GLuint)(text.GetTextFieldBuffer()->GetResourceSize());

		glBindVertexArray(textVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (GLuint)(text.GetFontTextureProxy()->GetResourceID()));

		// Render glyph texture over quad
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(6 * textBufferSize));

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
	// // CHECK_GL_ERR();

	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void TiledForwardRenderer::UIImgui()
{
	static bool IsImguiInit = false;
	
	// Doing rendering part before pass resources init
	// we avoid falling into rendering pass code just after
	// resource creation when frame is not yet started.
	if (IsImguiInit)
	{
		// Creating geometry.
		// Ending imgui frame after update of engine and game systems.
		ImGui::Render();
	
		ImDrawData* draw_data = ImGui::GetDrawData();
		if (draw_data == nullptr)
		{
			core::utils::gConsole.LogInfo("TiledForwardRenderer::UIImgui draw_data is null");
		}
		else
		{
			ImGuiIO& io = ImGui::GetIO();
			glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
			// drawing geometry to framebuffer
			ImGui_ImplOpenGL3_RenderDrawData(draw_data);
		}
	}
	
	// core::utils::gConsole.LogInfo("TiledForwardRenderer::UIImgui IsImguiInit: {}, GetCurrentContext: {}",
	// 	IsImguiInit, ImGui::GetCurrentContext() != nullptr);
	
	// ImguiSystem with input receiver and window drawing module
	// need initialized imgui font atlas on first frame. We create 
	// font atlas since render init is done before ResourceInit.
	if (!IsImguiInit && ImGui::GetCurrentContext() != nullptr)
	{
		core::utils::gConsole.LogInfo("TiledForwardRenderer::Render ImGui_ImplOpenGL3_CreateDeviceObjects");
		ImGui_ImplOpenGL3_CreateDeviceObjects();
		IsImguiInit = true;
	}
}

void TiledForwardRenderer::DebugLightAccum(const SceneView& sceneView)
{
	float time = (float)(sceneView.SceneData->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DebugLightAccumShader.BindProgram();
	static const core::storage::String uTime("uTime");
	DebugLightAccumShader.SetUniform(uTime, time);
	static const core::storage::String uWorkGroupsX("uWorkGroupsX");
	DebugLightAccumShader.SetUniform(uWorkGroupsX, (int)WorkGroupsX);
	static const core::storage::String uWorkGroupsY("uWorkGroupsY");
	DebugLightAccumShader.SetUniform(uWorkGroupsY, (int)WorkGroupsY);
	static const core::storage::String uLightCount("uLightCount");
	DebugLightAccumShader.SetUniform(uLightCount, (int)std::min((int)sceneView.PointLightList.size(), MAX_NUM_LIGHTS));

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, LightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, VisibleLightIndicesBuffer);

	const core::math::Matrix& clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();
	for (const auto& [meshCmp] : sceneView.SceneData->IterateComponents<MeshRenderingComponent>())
	{
		const EntityTransform& transform = meshCmp->GetTransform();
		const core::math::Matrix& worldFromModel = transform.GetWorldFromModel();
		static const core::storage::String uClipFromModel("uClipFromModel");
		DebugLightAccumShader.SetUniform(uClipFromModel, clipFromWorld * worldFromModel);
		static const core::storage::String uWorldFromModel("uWorldFromModel");
		DebugLightAccumShader.SetUniform(uWorldFromModel, worldFromModel);

		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, RDI->FallbackWhiteTexture);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

}

void TiledForwardRenderer::SetupLightsBufferFromScene()
{
	if (LightBuffer == 0)
	{
		return;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightBuffer);
	Light *lights = (Light*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < MAX_NUM_LIGHTS; ++i)
	{
		Light &light = lights[i];
		light.Position = core::math::Vector::ZERO;
		light.Color = core::math::Vector::ZERO;
		light.RangeIntensity = core::math::Vector(-1.0f, 0.0f, 0.0f);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::UpdateLightsBufferFromScene(const SceneView& sceneView)
{
	std::vector<core::math::Vector> positions;
	std::vector<core::math::Vector> color;
	std::vector<core::math::Vector> rangeIntensity;

	int lightCounter = 0;
	for (const PointLightComponent* pointLightCmp : sceneView.PointLightList)
	{
		const EntityTransform& transform = pointLightCmp->GetTransform();

		positions.push_back(transform.GetGlobalTranslation());
		color.push_back(core::math::Vector(pointLightCmp->GetColor()));
		rangeIntensity.push_back(core::math::Vector(pointLightCmp->GetRange(), pointLightCmp->GetIntensity(), 0.0f));

		++lightCounter;

		if (lightCounter > MAX_NUM_LIGHTS)
		{
			core::utils::gConsole.LogInfo("TiledForwardRenderer::UpdateLightsBufferFromScene more lights than supported by renderer({})", MAX_NUM_LIGHTS);
			break;
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightBuffer);
	Light *lights = (Light*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < lightCounter; ++i)
	{
		Light &light = lights[i];
		light.Position = positions[i];
		light.Color = color[i];
		light.RangeIntensity = rangeIntensity[i];

		// 		core::utils::gConsole.LogInfo("TiledForwardRenderer::UpdateLightsBufferFromScene set Position: {}, Radius: {}",
		// 			light.Position, light.Radius);
	}

	int remainingSlots = MAX_NUM_LIGHTS - lightCounter;
	for (int i = 0; i < remainingSlots; ++i)
	{
		Light &light = lights[lightCounter + i];
		light.Position = core::math::Vector::ZERO;
		light.Color = core::math::Vector::ZERO;
		light.RangeIntensity = core::math::Vector(-1.0f, 0.0f, 0.0f);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::DebugDepthPrepass(const SceneView& sceneView)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Weirdly, moving this call drops performance into the floor
	DebugQuadDepthPrepassShader.BindProgram();
	static const core::storage::String uNear("uNear");
	DebugQuadDepthPrepassShader.SetUniform(uNear, sceneView.CameraCmp->GetClippingPlaneNear());
	static const core::storage::String uFar("uFar");
	DebugQuadDepthPrepassShader.SetUniform(uFar, sceneView.CameraCmp->GetClippingPlaneFar());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PreDepthBuffer);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}