#include <PolyRenderingDeviceGLPCH.hpp>

#include <TiledForwardRenderer.hpp>
#include <Configs/AssetsPathConfig.hpp>

#include <GLRenderingDevice.hpp>
#include <Proxy/GLMeshDeviceProxy.hpp>
#include <Proxy/GLParticleDeviceProxy.hpp>
#include <Pipeline/RenderingPassBase.hpp>
#include <Common/DebugRenderingBuffers.hpp>
#include <Proxy/GLShaderProgram.hpp>
#include <Common/PrimitiveCube.hpp>
#include <Common/PrimitiveQuad.hpp>

#include "Proxy/imgui_impl_opengl3.h"

using namespace Poly;

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
	: IRendererInterface(rdi), SkyboxCapture(rdi),
	ShadowMapShader("Shaders/shadowMap.vert.glsl", "Shaders/shadowMap.frag.glsl"),
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
	ShadowMapShader.RegisterUniform("mat4", "uClipFromModel");

	LightAccumulationShader.RegisterUniform("float", "uTime");
	LightAccumulationShader.RegisterUniform("vec4", "uViewPosition");
	LightAccumulationShader.RegisterUniform("mat4", "uClipFromModel");
	LightAccumulationShader.RegisterUniform("mat4", "uWorldFromModel");
	LightAccumulationShader.RegisterUniform("vec4", "uMaterial.Emissive");
	LightAccumulationShader.RegisterUniform("vec4", "uMaterial.Albedo");
	LightAccumulationShader.RegisterUniform("float", "uMaterial.Roughness");
	LightAccumulationShader.RegisterUniform("float", "uMaterial.Metallic");

	LightAccumulationShader.RegisterUniform("sampler2D", "uBrdfLUT");
	LightAccumulationShader.RegisterUniform("samplerCube", "uIrradianceMap");
	LightAccumulationShader.RegisterUniform("samplerCube", "uPrefilterMap");

	LightAccumulationShader.RegisterUniform("sampler2D", "uEmissiveMap");
	LightAccumulationShader.RegisterUniform("sampler2D", "uAlbedoMap");
	LightAccumulationShader.RegisterUniform("sampler2D", "uRoughnessMap");
	LightAccumulationShader.RegisterUniform("sampler2D", "uMetallicMap");
	LightAccumulationShader.RegisterUniform("sampler2D", "uNormalMap");
	LightAccumulationShader.RegisterUniform("sampler2D", "uAmbientOcclusionMap");
	LightAccumulationShader.RegisterUniform("sampler2D", "uDirShadowMap");
	
	for (int i = 0; i < 8; ++i)
	{
		String baseName = String("uDirectionalLight[") + String::From(i) + String("].");
		LightAccumulationShader.RegisterUniform("vec4", baseName + "ColorIntensity");
		LightAccumulationShader.RegisterUniform("vec4", baseName + "Direction");
	}
	LightAccumulationShader.RegisterUniform("int", "uDirectionalLightCount");
	LightAccumulationShader.RegisterUniform("int", "uLightCount");
	LightAccumulationShader.RegisterUniform("int", "uWorkGroupsX");
	LightAccumulationShader.RegisterUniform("int", "uWorkGroupsY");

	HDRShader.RegisterUniform("sampler2D", "uHdrBuffer");
	HDRShader.RegisterUniform("float", "uExposure");
	
	MotionBlurShader.RegisterUniform("sampler2D", "uImage");
	MotionBlurShader.RegisterUniform("sampler2D", "uDepth");
	MotionBlurShader.RegisterUniform("mat4", "uWorldFromView");
	MotionBlurShader.RegisterUniform("mat4", "uPrevClipFromWorld");
	MotionBlurShader.RegisterUniform("float", "uScale");

	DOFBokehShader.RegisterUniform("vec4", "uRes");
	DOFBokehShader.RegisterUniform("sampler2D", "uImage");
	DOFBokehShader.RegisterUniform("sampler2D", "uDepth");
	DOFBokehShader.RegisterUniform("float", "uTime");
	DOFBokehShader.RegisterUniform("float", "uDOFPoint");
	DOFBokehShader.RegisterUniform("float", "uDOFRange");
	DOFBokehShader.RegisterUniform("float", "uDOFSize");

	DOFApplyShader.RegisterUniform("sampler2D", "uImage");
	DOFApplyShader.RegisterUniform("sampler2D", "uDOF");
	DOFApplyShader.RegisterUniform("float", "uDOFShow");
	DOFApplyShader.RegisterUniform("float", "uDOFPoint");
	DOFApplyShader.RegisterUniform("float", "uDOFRange");

	BloomBrightShader.RegisterUniform("sampler2D", "uImage");
	BloomBrightShader.RegisterUniform("float", "uBrightThreshold");

	BloomBlurShader.RegisterUniform("float", "uIsHorizontal");
	BloomBlurShader.RegisterUniform("sampler2D", "uImage");

	BloomApplyShader.RegisterUniform("sampler2D", "uImage");
	BloomApplyShader.RegisterUniform("sampler2D", "uBloom");
	BloomApplyShader.RegisterUniform("float", "uBloomScale");

	SkyboxShader.RegisterUniform("mat4", "uClipFromWorld");
	SkyboxShader.RegisterUniform("vec4", "uTint");

	TranslucentShader.RegisterUniform("float", "uTime");
	TranslucentShader.RegisterUniform("vec4", "uViewPosition");
	TranslucentShader.RegisterUniform("mat4", "uClipFromModel");
	TranslucentShader.RegisterUniform("mat4", "uWorldFromModel");
	TranslucentShader.RegisterUniform("vec4", "uMaterial.Emissive");
	TranslucentShader.RegisterUniform("vec4", "uMaterial.Albedo");
	TranslucentShader.RegisterUniform("float", "uMaterial.Roughness");
	TranslucentShader.RegisterUniform("float", "uMaterial.Metallic");

	TranslucentShader.RegisterUniform("sampler2D", "uBrdfLUT");
	TranslucentShader.RegisterUniform("samplerCube", "uIrradianceMap");
	TranslucentShader.RegisterUniform("samplerCube", "uPrefilterMap");

	TranslucentShader.RegisterUniform("sampler2D", "uEmissiveMap");
	TranslucentShader.RegisterUniform("sampler2D", "uAlbedoMap");
	TranslucentShader.RegisterUniform("sampler2D", "uRoughnessMap");
	TranslucentShader.RegisterUniform("sampler2D", "uMetallicMap");
	TranslucentShader.RegisterUniform("sampler2D", "uNormalMap");
	TranslucentShader.RegisterUniform("sampler2D", "uAmbientOcclusionMap");	

	for (int i = 0; i < 8; ++i)
	{
		String baseName = String("uDirectionalLight[") + String::From(i) + String("].");
		TranslucentShader.RegisterUniform("vec4", baseName + "ColorIntensity");
		TranslucentShader.RegisterUniform("vec4", baseName + "Direction");
	}
	TranslucentShader.RegisterUniform("int", "uDirectionalLightCount");
	TranslucentShader.RegisterUniform("int", "uLightCount");
	TranslucentShader.RegisterUniform("int", "uWorkGroupsX");
	TranslucentShader.RegisterUniform("int", "uWorkGroupsY");

	ParticleShader.RegisterUniform("float", "uTime");
	ParticleShader.RegisterUniform("mat4", "uScreenFromView");
	ParticleShader.RegisterUniform("mat4", "uViewFromWorld");
	ParticleShader.RegisterUniform("mat4", "uWorldFromModel");
	ParticleShader.RegisterUniform("vec4", "uColor");

	LinearizeDepthShader.RegisterUniform("float", "uNear");
	LinearizeDepthShader.RegisterUniform("float", "uFar");

	EditorDebugShader.RegisterUniform("mat4", "uMVP");

	GammaShader.RegisterUniform("sampler2D", "uSplashImage");
	GammaShader.RegisterUniform("vec4", "uSplashTint");
	GammaShader.RegisterUniform("sampler2D", "uImage");
	GammaShader.RegisterUniform("float", "uTime");
	GammaShader.RegisterUniform("vec4", "uRes");
	GammaShader.RegisterUniform("float", "uGrainScale");
	GammaShader.RegisterUniform("float", "uVignetteScale");
	GammaShader.RegisterUniform("float", "uAbberationScale");
	GammaShader.RegisterUniform("float", "uGamma");
}

void TiledForwardRenderer::Init()
{
	gConsole.LogInfo("TiledForwardRenderer::Init");
	
	const ScreenSize screenSize = RDI->GetScreenSize();

	gConsole.LogInfo("TiledForwardRenderer::Init SCREEN_SIZE: ({},{})", screenSize.Width, screenSize.Height);

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
	gConsole.LogInfo("TiledForwardRenderer::Resize ({}, {})", size.Width, size.Height);

	DeleteLightBuffers();

	DeleteRenderTargets();

	CreateLightBuffers(size);

	CreateRenderTargets(size);
}

void TiledForwardRenderer::Deinit()
{
	gConsole.LogInfo("TiledForwardRenderer::Deinit");
	
	// Imgui context is needed to propertly deinit textures.
	// Deinit is called after engine resources we need  destroy
	// imgui pass textures and objects and then end Imgui context
	// started in ImguiResource owned by ImguiWorldComponent
	ImGui_ImplOpenGL3_DestroyDeviceObjects();
	ImGui::DestroyContext();

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
	gConsole.LogInfo("TiledForwardRenderer::PreintegrateBRDF Create texture resource");
	glGenTextures(1, &PreintegratedBrdfLUT);
	glBindTexture(GL_TEXTURE_2D, PreintegratedBrdfLUT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	// CHECK_GL_ERR();

	// Create FBO for BRDF preintegration
	gConsole.LogInfo("TiledForwardRenderer::PreintegrateBRDF Create capture FBO");
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	gConsole.LogInfo("TiledForwardRenderer::PreintegrateBRDF Capture FBO created");

	// Render preintegrated BRDF
	gConsole.LogInfo("TiledForwardRenderer::PreintegrateBRDF Render preintegrated BRDF");
	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	IntegrateBRDFShader.BindProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PreintegratedBrdfLUT, 0);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap preintegrated BRDF rendered");
}

void TiledForwardRenderer::CreateLightBuffers(const ScreenSize& size)
{
	// Define work group sizes in x and y direction based off screen size and tile size (in pixels)

	WorkGroupsX = (size.Width + (size.Width % 16)) / 16;
	WorkGroupsY = (size.Height + (size.Height % 16)) / 16;
	size_t numberOfTiles = WorkGroupsX * WorkGroupsY;
	gConsole.LogInfo("TiledForwardRenderer::Init workGroups: ({},{}), numberOfTiles: {}", WorkGroupsX, WorkGroupsY, numberOfTiles);

	// Generate our shader storage buffers
	glGenBuffers(1, &LightBuffer);
	glGenBuffers(1, &VisibleLightIndicesBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * MAX_NUM_LIGHTS, 0, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, VisibleLightIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VisibleIndex) * MAX_NUM_LIGHTS * numberOfTiles, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// CHECK_GL_ERR();
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

	// CHECK_GL_ERR();
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

	// CHECK_GL_ERR();
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

	// CHECK_GL_ERR();
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

	// CHECK_GL_ERR();
	CHECK_FBO_STATUS();

	glGenTextures(1, &DirShadowMap);
	glBindTexture(GL_TEXTURE_2D, DirShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float shadowBorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, shadowBorderColor);

	glGenFramebuffers(1, &FBOShadowDepthMap);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowDepthMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DirShadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// CHECK_GL_ERR();
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

	if (FBOShadowDepthMap > 0)
		glDeleteFramebuffers(1, &FBOShadowDepthMap);

	RTBloom.Deinit();

	// CHECK_GL_ERR();
	CHECK_FBO_STATUS();
}

void TiledForwardRenderer::Render(const SceneView& sceneView)
{
	//gConsole.LogInfo("TiledForwardRenderer::Render");

	// gConsole.LogInfo("TiledForwardRenderer::Render Aspect: {}, IsForcedRatio: {}, FOV: {}",
	// 	sceneView.CameraCmp->GetAspect(), sceneView.CameraCmp->GetForcedRatio(), sceneView.CameraCmp->GetFOV());

	// glViewport((int)(sceneView.Rect.GetMin().X * screenSize.Width), (int)(sceneView.Rect.GetMin().Y * screenSize.Height),
	// 	(int)(sceneView.Rect.GetSize().X * screenSize.Width), (int)(sceneView.Rect.GetSize().Y * screenSize.Height));  

	UpdateLightsBufferFromScene(sceneView);
	
	UpdateEnvCapture(sceneView);

	RenderShadowMap(sceneView);
	
	RenderDepthPrePass(sceneView);
	
	ComputeLightCulling(sceneView);
	
	RenderOpaqueLit(sceneView);
	
	RenderSkybox(sceneView);

	RenderTranslucentLit(sceneView);

	RenderParticleUnlit(sceneView.WorldData, sceneView.CameraCmp);

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
	PreviousFrameCameraClipFromWorld = Matrix(sceneView.CameraCmp->GetClipFromWorld().GetDataPtr());
	PreviousFrameCameraTransform = Matrix(sceneView.CameraCmp->GetViewFromWorld().GetDataPtr());
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
			gConsole.LogInfo("TiledForwardRenderer::Render draw_data is null");
		}
		else
		{
			ImGuiIO& io = ImGui::GetIO();
			glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
			// drawing geometry to framebuffer
			ImGui_ImplOpenGL3_RenderDrawData(draw_data);
		}
	}

	gConsole.LogInfo("TiledForwardRenderer::Render IsImguiInit: {}, GetCurrentContext: {}",
		IsImguiInit, ImGui::GetCurrentContext() != nullptr);

	// ImguiSystem with input receiver and window drawing module
	// need initialized imgui font atlas on first frame. We create 
	// font atlas since render init is done before ResourceInit.
	if (!IsImguiInit && ImGui::GetCurrentContext() != nullptr)
	{
		gConsole.LogInfo("TiledForwardRenderer::Render CreateDeviceObjects");
		ImGui_ImplOpenGL3_CreateDeviceObjects();
		IsImguiInit = true;
	}
}

void TiledForwardRenderer::UpdateEnvCapture(const SceneView& sceneView)
{
	// gConsole.LogInfo("TiledForwardRenderer::UpdateEnvCapture");

	if (SkyboxCapture.GetIsDirty())
	{
		const SkyboxWorldComponent* SkyboxWorldCmp = sceneView.WorldData->GetWorldComponent<SkyboxWorldComponent>();
		if (SkyboxWorldCmp != nullptr)
		{
			SkyboxCapture.UpdateEnv(SkyboxWorldCmp);
		}
		else
		{
			gConsole.LogInfo("TiledForwardRenderer::UpdateEnvCapture SkyboxWorldComponent not found!");
		}
	}
}

void TiledForwardRenderer::RenderEquiCube(const SceneView& sceneView)
{
	const Matrix clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();

	EquiToCubemapShader.BindProgram();
	EquiToCubemapShader.SetUniform("uClipFromModel", clipFromWorld);

	// GLuint CubemapID = static_cast<const GLCubemapDeviceProxy*>(SkyboxWorldCmp->GetCubemap().GetTextureProxy(->GetResourceID());

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glBindFragDataLocation((GLuint)EquiToCubemapShader.GetProgramHandle(), (GLuint)0, "color");
	glBindFragDataLocation((GLuint)EquiToCubemapShader.GetProgramHandle(), (GLuint)1, "normal");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, SkyboxCapture.GetHDRPanorama());
	EquiToCubemapShader.SetUniform("uEquirectangularMap", 0);

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

Matrix TiledForwardRenderer::GetProjectionForShadowMap(const DirectionalLightComponent* dirLightCmp) const
{
	// TODO: calc bounding box and then determine projection size
	// make sure contains all the objects
	float near_plane = -4096.0f, far_plane = 4096.0f;
	Matrix dirLightProjection;
	dirLightProjection.SetOrthographic(-4096.0f, 4096.0f, -4096.0f, 4096.0f, near_plane, far_plane);
	
	Matrix dirLightFromWorld = dirLightCmp->GetTransform().GetWorldFromModel().GetInversed();
	return dirLightFromWorld * dirLightProjection;
}

void TiledForwardRenderer::RenderShadowMap(const SceneView& sceneView)
{
	if (sceneView.DirectionalLights.GetSize() < 1)
		return;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOShadowDepthMap);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	Matrix projDirLightFromWorld = GetProjectionForShadowMap(sceneView.DirectionalLights[0]);
	
	ShadowMapShader.BindProgram();

	for (const MeshRenderingComponent* meshCmp : sceneView.OpaqueQueue)
	{
		const Matrix& worldFromModel = meshCmp->GetTransform().GetWorldFromModel();
		ShadowMapShader.SetUniform("uClipFromModel", projDirLightFromWorld * worldFromModel);

		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{			
			glBindVertexArray(subMesh->GetMeshProxy()->GetResourceID());

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}

	for (const MeshRenderingComponent* meshCmp : sceneView.DirShadowOpaqueQueue)
	{
		const Matrix& worldFromModel = meshCmp->GetTransform().GetWorldFromModel();
		ShadowMapShader.SetUniform("uClipFromModel", projDirLightFromWorld * worldFromModel);

		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			glBindVertexArray(subMesh->GetMeshProxy()->GetResourceID());

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);
}

void TiledForwardRenderer::RenderDepthPrePass(const SceneView& sceneView)
{
	ScreenSize screenSize = RDI->GetScreenSize();
	glViewport(0, 0, screenSize.Width, screenSize.Height);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOdepthMap);
	glClear(GL_DEPTH_BUFFER_BIT);

	DepthShader.BindProgram();
	
//	const ScreenSize screenSize = RDI->GetScreenSize();
// 	glViewport((int)(sceneView.Rect.GetMin().X * screenSize.Width), (int)(sceneView.Rect.GetMin().Y * screenSize.Height),
// 		(int)(sceneView.Rect.GetSize().X * screenSize.Width), (int)(sceneView.Rect.GetSize().Y * screenSize.Height));

	const Matrix& clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();
	
	for (const MeshRenderingComponent* meshCmp : sceneView.OpaqueQueue)
	{
		const Matrix& worldFromModel = meshCmp->GetTransform().GetWorldFromModel();
		DepthShader.SetUniform("uScreenFromModel", clipFromWorld * worldFromModel);
		
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
	// gConsole.LogInfo("TiledForwardRenderer::LightCulling Time: {}, workGroupsX: {}, workGroupsY: {}",
	// 	Time, workGroupsX, workGroupsY
	// );

	LightCullingShader.BindProgram();
	LightCullingShader.SetUniform("uViewFromWorld", sceneView.CameraCmp->GetViewFromWorld());
	LightCullingShader.SetUniform("uClipFromWorld", sceneView.CameraCmp->GetClipFromWorld());
	LightCullingShader.SetUniform("uClipFromView",  sceneView.CameraCmp->GetClipFromView());
	LightCullingShader.SetUniform("uScreenSizeX", RDI->GetScreenSize().Width);
	LightCullingShader.SetUniform("uScreenSizeY", RDI->GetScreenSize().Height);
	LightCullingShader.SetUniform("uWorkGroupsX", (int)WorkGroupsX);
	LightCullingShader.SetUniform("uWorkGroupsY", (int)WorkGroupsY);
	LightCullingShader.SetUniform("uLightCount", (int)std::min((int)sceneView.PointLights.GetSize(), MAX_NUM_LIGHTS));

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
	// gConsole.LogInfo("TiledForwardRenderer::AccumulateLights");
	float time = (float)TimeSystem::GetTimerElapsedTime(sceneView.WorldData, eEngineTimer::GAMEPLAY);

	ScreenSize screenSize = RDI->GetScreenSize();
	glViewport(0, 0, screenSize.Width, screenSize.Height);

	for (int i = 9; i > 0; --i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	LightAccumulationShader.BindProgram();

	Matrix projDirLightFromWorld = sceneView.DirectionalLights.IsEmpty() ? Matrix() : GetProjectionForShadowMap(sceneView.DirectionalLights[0]);

	LightAccumulationShader.SetUniform("uDirLightFromWorld", projDirLightFromWorld);
	LightAccumulationShader.BindSampler("uDirShadowMap", 9, DirShadowMap);

	const EntityTransform& cameraTransform = sceneView.CameraCmp->GetTransform();

	LightAccumulationShader.SetUniform("uTime", time);
	LightAccumulationShader.SetUniform("uLightCount", (int)std::min((int)sceneView.PointLights.GetSize(), MAX_NUM_LIGHTS));
	LightAccumulationShader.SetUniform("uWorkGroupsX", (int)WorkGroupsX);
	LightAccumulationShader.SetUniform("uWorkGroupsY", (int)WorkGroupsY);
	LightAccumulationShader.SetUniform("uViewPosition", cameraTransform.GetGlobalTranslation());

	int dirLightsCount = 0;
	for (const DirectionalLightComponent* dirLightCmp : sceneView.DirectionalLights)
	{		
		const EntityTransform& transform = dirLightCmp->GetTransform();
		String baseName = String("uDirectionalLight[") + String::From(dirLightsCount) + String("].");
		Color colorIntensity = dirLightCmp->GetColor();
		colorIntensity.A = dirLightCmp->GetIntensity();
		LightAccumulationShader.SetUniform(baseName + "ColorIntensity", colorIntensity);
		LightAccumulationShader.SetUniform(baseName + "Direction", MovementSystem::GetGlobalForward(transform));

		++dirLightsCount;
		if (dirLightsCount == MAX_LIGHT_COUNT_DIRECTIONAL)
			break;
	}
	LightAccumulationShader.SetUniform("uDirectionalLightCount", dirLightsCount);
	
	LightAccumulationShader.BindSamplerCube("uIrradianceMap", 0, SkyboxCapture.GetIrradianceMap());
	LightAccumulationShader.BindSamplerCube("uPrefilterMap", 1, SkyboxCapture.GetPrefilterMap());
	LightAccumulationShader.BindSampler("uBrdfLUT", 2, PreintegratedBrdfLUT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, LightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, VisibleLightIndicesBuffer);
	
	glBindFragDataLocation((GLuint)LightAccumulationShader.GetProgramHandle(), 0, "oColor");
	glBindFragDataLocation((GLuint)LightAccumulationShader.GetProgramHandle(), 1, "oNormal");

	const Matrix& clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();
	for (const MeshRenderingComponent* meshCmp : sceneView.OpaqueQueue)
	{
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& worldFromModel = transform.GetWorldFromModel();
		LightAccumulationShader.SetUniform("uClipFromModel", clipFromWorld * worldFromModel);
		LightAccumulationShader.SetUniform("uWorldFromModel", worldFromModel);
		
		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			Material material = meshCmp->GetMaterial(i);
			LightAccumulationShader.SetUniform("uMaterial.Emissive", material.Emissive);
			LightAccumulationShader.SetUniform("uMaterial.Albedo", material.Albedo);
			LightAccumulationShader.SetUniform("uMaterial.Roughness", material.Roughness); 
			LightAccumulationShader.SetUniform("uMaterial.Metallic", material.Metallic);
			LightAccumulationShader.SetUniform("uMaterial.OpacityMaskThreshold", material.OpacityMaskThreshold);

			const TextureResource* emissiveMap = subMesh->GetMeshData().GetEmissiveMap();
			const TextureResource* albedoMap = subMesh->GetMeshData().GetAlbedoMap();
			const TextureResource* roughnessMap = subMesh->GetMeshData().GetRoughnessMap();
			const TextureResource* metallicMap = subMesh->GetMeshData().GetMetallicMap();
			const TextureResource* normalMap = subMesh->GetMeshData().GetNormalMap();
			const TextureResource* ambientOcclusionMap = subMesh->GetMeshData().GetAmbientOcclusionMap();

			// Material textures
			LightAccumulationShader.BindSampler("uEmissiveMap",			3, emissiveMap			? emissiveMap->GetTextureProxy()->GetResourceID()			: RDI->FallbackWhiteTexture);
			LightAccumulationShader.BindSampler("uAlbedoMap",			4, albedoMap			? albedoMap->GetTextureProxy()->GetResourceID()				: RDI->FallbackWhiteTexture);
			LightAccumulationShader.BindSampler("uRoughnessMap",		5, roughnessMap			? roughnessMap->GetTextureProxy()->GetResourceID()			: RDI->FallbackWhiteTexture);
			LightAccumulationShader.BindSampler("uMetallicMap",			6, metallicMap			? metallicMap->GetTextureProxy()->GetResourceID()			: RDI->FallbackWhiteTexture);
			LightAccumulationShader.BindSampler("uNormalMap",			7, normalMap			? normalMap->GetTextureProxy()->GetResourceID()				: RDI->FallbackNormalMap);
			LightAccumulationShader.BindSampler("uAmbientOcclusionMap",	8, ambientOcclusionMap	? ambientOcclusionMap->GetTextureProxy()->GetResourceID()	: RDI->FallbackWhiteTexture);

			const GLuint subMeshVAO = (GLuint)(subMesh->GetMeshProxy()->GetResourceID());
			glBindVertexArray(subMeshVAO);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			++i;
		}
	}
	
	// CHECK_GL_ERR();

	// Clear bound resources
	glBindVertexArray(0);

	for (int i = 9; i > 0; --i)
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
	if (SkyboxCapture.GetEnvCubemap() > 0)
	{
		Color tint = Color::WHITE;
		SkyboxWorldComponent* skyboxCmp = sceneView.WorldData->GetWorldComponent<SkyboxWorldComponent>();
		if (skyboxCmp)
		{
			tint = skyboxCmp->GetTint();
		}

		const Matrix clipFromView = sceneView.CameraCmp->GetClipFromView();
		Matrix viewFromWorld = sceneView.CameraCmp->GetViewFromWorld();
		// center cube in view space by setting translation to 0 for x, y and z.
		// SetTranslation resets Matrix to identity
		viewFromWorld.Data[3] = 0.0f;
		viewFromWorld.Data[7] = 0.0f;
		viewFromWorld.Data[11] = 0.0f;

		Matrix clipFromWorld = clipFromView * viewFromWorld;

		glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);
		
		SkyboxShader.BindProgram();
		SkyboxShader.SetUniform("uTint", tint);
		SkyboxShader.SetUniform("uClipFromWorld", clipFromWorld);
		SkyboxShader.BindSamplerCube("uCubemap", 0, SkyboxCapture.GetEnvCubemap());
		
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
	// gConsole.LogInfo("TiledForwardRenderer::RenderTranslucenLit");
	
	for (int i = 8; i > 0; --i)
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
	TranslucentShader.SetUniform("uLightCount", (int)std::min((int)sceneView.PointLights.GetSize(), MAX_NUM_LIGHTS));
	TranslucentShader.SetUniform("uWorkGroupsX", (int)WorkGroupsX);
	TranslucentShader.SetUniform("uWorkGroupsY", (int)WorkGroupsY);
	TranslucentShader.SetUniform("uViewPosition", cameraTransform.GetGlobalTranslation());

	int dirLightsCount = 0;
	for (const DirectionalLightComponent* dirLightCmp : sceneView.DirectionalLights)
	{
		const EntityTransform& transform = dirLightCmp->GetTransform();
		String baseName = String("uDirectionalLight[") + String::From(dirLightsCount) + String("].");
		Color colorIntensity = dirLightCmp->GetColor();
		colorIntensity.A = dirLightCmp->GetIntensity();
		TranslucentShader.SetUniform(baseName + "ColorIntensity", colorIntensity);
		TranslucentShader.SetUniform(baseName + "Direction", MovementSystem::GetGlobalForward(transform));

		++dirLightsCount;
		if (dirLightsCount == MAX_LIGHT_COUNT_DIRECTIONAL)
			break;
	}
	TranslucentShader.SetUniform("uDirectionalLightCount", dirLightsCount);

	TranslucentShader.BindSamplerCube("uIrradianceMap", 0, SkyboxCapture.GetIrradianceMap());
	TranslucentShader.BindSamplerCube("uPrefilterMap", 1, SkyboxCapture.GetPrefilterMap());
	TranslucentShader.BindSampler("uBrdfLUT", 2, PreintegratedBrdfLUT);

	glBindFragDataLocation((GLuint)TranslucentShader.GetProgramHandle(), 0, "oColor");
	glBindFragDataLocation((GLuint)TranslucentShader.GetProgramHandle(), 1, "oNormal");

	const Matrix& clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();
	for (const MeshRenderingComponent* meshCmp : sceneView.TranslucentQueue)
	{
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& worldFromModel = transform.GetWorldFromModel();
		TranslucentShader.SetUniform("uClipFromModel", clipFromWorld * worldFromModel);
		TranslucentShader.SetUniform("uWorldFromModel", worldFromModel);

		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			Material material = meshCmp->GetMaterial(i);
			TranslucentShader.SetUniform("uMaterial.Emissive", material.Emissive);
			TranslucentShader.SetUniform("uMaterial.Albedo", material.Albedo);
			TranslucentShader.SetUniform("uMaterial.Roughness", material.Roughness);
			TranslucentShader.SetUniform("uMaterial.Metallic", material.Metallic);
			TranslucentShader.SetUniform("uMaterial.OpacityMaskThreshold", material.OpacityMaskThreshold);

			const TextureResource* emissiveMap = subMesh->GetMeshData().GetEmissiveMap();
			const TextureResource* albedoMap = subMesh->GetMeshData().GetAlbedoMap();
			const TextureResource* roughnessMap = subMesh->GetMeshData().GetRoughnessMap();
			const TextureResource* metallicMap = subMesh->GetMeshData().GetMetallicMap();
			const TextureResource* normalMap = subMesh->GetMeshData().GetNormalMap();
			const TextureResource* ambientOcclusionMap = subMesh->GetMeshData().GetAmbientOcclusionMap();

			// Material textures
			TranslucentShader.BindSampler("uEmissiveMap",			3, emissiveMap			? emissiveMap->GetTextureProxy()->GetResourceID()			: RDI->FallbackBlackTexture);
			TranslucentShader.BindSampler("uAlbedoMap",				4, albedoMap			? albedoMap->GetTextureProxy()->GetResourceID()				: RDI->FallbackWhiteTexture);
			TranslucentShader.BindSampler("uRoughnessMap",			5, roughnessMap			? roughnessMap->GetTextureProxy()->GetResourceID()			: RDI->FallbackWhiteTexture);
			TranslucentShader.BindSampler("uMetallicMap",			6, metallicMap			? metallicMap->GetTextureProxy()->GetResourceID()			: RDI->FallbackWhiteTexture);
			TranslucentShader.BindSampler("uNormalMap",				7, normalMap			? normalMap->GetTextureProxy()->GetResourceID()				: RDI->FallbackNormalMap);
			TranslucentShader.BindSampler("uAmbientOcclusionMap",	8, ambientOcclusionMap	? ambientOcclusionMap->GetTextureProxy()->GetResourceID()	: RDI->FallbackWhiteTexture);

			const GLuint subMeshVAO = (GLuint)(subMesh->GetMeshProxy()->GetResourceID());
			glBindVertexArray(subMeshVAO);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			++i;
		}
	}
	
	// CHECK_GL_ERR();

	// Clear bound resources
	glBindVertexArray(0);

	for (int i = 8; i > 0; --i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// CHECK_GL_ERR();
}

void TiledForwardRenderer::RenderParticleUnlit(Scene* world, const CameraComponent* cameraCmp)
{
	// gConsole.LogInfo("TiledForwardRenderer::RenderParticleUnlit");

	float time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	const Matrix& viewFromWorld = cameraCmp->GetViewFromWorld();
	const Matrix& screenFromView = cameraCmp->GetClipFromView();

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	ParticleShader.BindProgram();
	ParticleShader.SetUniform("uTime", time);
	ParticleShader.SetUniform("uScreenFromView", screenFromView);

	glBindFragDataLocation((GLuint)TranslucentShader.GetProgramHandle(), 0, "color");
	glBindFragDataLocation((GLuint)TranslucentShader.GetProgramHandle(), 1, "normal");

	for (auto componentsTuple : world->IterateComponents<ParticleComponent>())
	{
		const ParticleComponent* particleCmp = std::get<ParticleComponent*>(componentsTuple);
		const EntityTransform& transform = particleCmp->GetTransform();
		const Matrix& worldFromModel = particleCmp->GetEmitter()->GetSettings().SimulationSpace == ParticleEmitter::eSimulationSpace::LOCAL_SPACE
			? transform.GetWorldFromModel()
			: Matrix();

		ParticleShader.SetUniform("uViewFromWorld", viewFromWorld);
		ParticleShader.SetUniform("uWorldFromModel", worldFromModel);

		ParticleEmitter::Settings emitterSettings = particleCmp->GetEmitter()->GetSettings();
		ParticleShader.SetUniform("uEmitterAlbedo", emitterSettings.Albedo);
		ParticleShader.SetUniform("uEmitterEmissive", emitterSettings.Emissive);

		SpritesheetSettings spriteSettings = emitterSettings.Spritesheet;
		ParticleShader.SetUniform("uSpriteColor", spriteSettings.SpriteColor);
		float startFrame = spriteSettings.IsRandomStartFrame ? RandomRange(0.0f, spriteSettings.SubImages.X * spriteSettings.SubImages.Y) : spriteSettings.StartFrame;
		ParticleShader.SetUniform("uSpriteStartFrame", startFrame);
		ParticleShader.SetUniform("uSpriteSpeed", spriteSettings.Speed);
		ParticleShader.SetUniform("uSpriteSubImages", spriteSettings.SubImages.X, spriteSettings.SubImages.Y);

		GLsizei partileLen = (GLsizei)(particleCmp->GetEmitter()->GetInstancesCount());
		const TextureResource* texture = particleCmp->GetEmitter()->GetSpritesheet();
		const GLParticleDeviceProxy* particleProxy = static_cast<const GLParticleDeviceProxy*>(particleCmp->GetEmitter()->GetParticleProxy());
		GLuint particleVAO = particleProxy->GetVAO();

		GLuint textureID = texture == nullptr
			? RDI->FallbackWhiteTexture
			: (GLuint)(texture->GetTextureProxy()->GetResourceID());

		ParticleShader.SetUniform("uHasSprite", texture == nullptr ? 0.0f : 1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

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

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LinearDepth, 0);

	LinearizeDepthShader.BindProgram();
	LinearizeDepthShader.BindSampler("uDepth", 0, PreDepthBuffer);
	LinearizeDepthShader.SetUniform("uNear", sceneView.CameraCmp->GetClippingPlaneNear());
	LinearizeDepthShader.SetUniform("uFar", sceneView.CameraCmp->GetClippingPlaneFar());

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

	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(sceneView.WorldData, Poly::eEngineTimer::GAMEPLAY));
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

	MotionBlurShader.BindProgram();
	MotionBlurShader.BindSampler("uImage", 0, ColorBuffer);
	MotionBlurShader.BindSampler("uDepth", 1, LinearDepth);
	MotionBlurShader.SetUniform("uPrevClipFromWorld", PreviousFrameCameraClipFromWorld);
	MotionBlurShader.SetUniform("uWorldFromView", sceneView.CameraCmp->GetViewFromWorld().GetInversed());
	MotionBlurShader.SetUniform("uScale", (currentFPS / targetFPS) * motionBlurScale * jitterFix);
	 
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
	if (postCmp) {
		dofPoint = postCmp->DOFPoint;
		dofRange = postCmp->DOFRange;
		dofSize = postCmp->DOFSize;
		dofShow = postCmp->DOFShow;
	}

	const ScreenSize screenSize = RDI->GetScreenSize();

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostColorBufferHalfRes, 0);

	glViewport(0, 0, screenSize.Width / 2, screenSize.Height / 2);

	DOFBokehShader.BindProgram();
	DOFBokehShader.SetUniform("uRes", Vector(0.5f * (float)screenSize.Width, 0.5f * (float)screenSize.Height, 2.0f / screenSize.Width, 2.0f / screenSize.Height));
	DOFBokehShader.SetUniform("uDOFPoint", dofPoint);
	DOFBokehShader.SetUniform("uDOFRange", dofRange);
	DOFBokehShader.SetUniform("uDOFSize", dofSize);
	DOFBokehShader.BindSampler("uImage", 0, PostColorBuffer1);
	DOFBokehShader.BindSampler("uDepth", 1, LinearDepth);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	glViewport(0, 0, screenSize.Width, screenSize.Height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostColorBuffer0, 0);

	DOFApplyShader.BindProgram();
	DOFApplyShader.BindSampler("uImage", 0, PostColorBuffer1);
	DOFApplyShader.BindSampler("uDOF", 1, PostColorBufferHalfRes);
	DOFApplyShader.BindSampler("uDepth", 2, LinearDepth);
	DOFApplyShader.SetUniform("uDOFPoint", dofPoint);
	DOFApplyShader.SetUniform("uDOFRange", dofRange);
	DOFApplyShader.SetUniform("uDOFShow", dofShow);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostBloom(const SceneView& sceneView)
{
	float bloomThreshold = 1.0f;
	float bloomScale = 0.1f;
	const PostprocessSettingsComponent* postCmp = sceneView.CameraCmp->GetSibling<PostprocessSettingsComponent>();
	if (postCmp) {
		bloomThreshold = postCmp->BloomThreshold;
		bloomScale = postCmp->BloomScale;
	}

	const ScreenSize screenSize = RDI->GetScreenSize();
	glViewport(0, 0, screenSize.Width / 2, screenSize.Height / 2);

	glBindFramebuffer(GL_FRAMEBUFFER, RTBloom.GetWriteFBO());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RTBloom.GetWriteTarget(), 0);

	BloomBrightShader.BindProgram();
	BloomBrightShader.SetUniform("uBrightThreshold", bloomThreshold);
	BloomBrightShader.BindSampler("uImage", 0, PostColorBuffer0);
	// BloomBrightShader.BindSampler("uImage", 0, ColorBuffer);
	
	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	RTBloom.Flip();
	
	BloomBlurShader.BindProgram();
	for (size_t i = 0; i < 6; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, RTBloom.GetWriteFBO());
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RTBloom.GetWriteTarget(), 0);

		BloomBlurShader.BindSampler("uImage", 0, RTBloom.GetReadTarget());
		BloomBlurShader.SetUniform("uIsHorizontal", (i % 2 > 0) ? 1.0f : 0.0f);

		glBindVertexArray(RDI->PrimitivesQuad->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		RTBloom.Flip();
	}
	
	glViewport(0, 0, screenSize.Width, screenSize.Height);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostColorBuffer1, 0);

	BloomApplyShader.BindProgram();
	BloomApplyShader.BindSampler("uImage", 0, PostColorBuffer0);
	BloomApplyShader.BindSampler("uBloom", 1, RTBloom.GetReadTarget());
	BloomApplyShader.SetUniform("uBloomScale", bloomScale);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostTonemapper(const SceneView& sceneView)
{
	// gConsole.LogInfo("TiledForwardRenderer::PostTonemapper");

	float exposure = 1.0f;
	const PostprocessSettingsComponent* postCmp = sceneView.CameraCmp->GetSibling<PostprocessSettingsComponent>();
	if (postCmp) {
		exposure = postCmp->Exposure;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost0);

	HDRShader.BindProgram();
	HDRShader.BindSampler("uHdrBuffer", 0, PostColorBuffer1);
	HDRShader.SetUniform("uExposure", exposure);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostFXAA(const SceneView& sceneView)
{
	// gConsole.LogInfo("TiledForwardRenderer::PostFXAA");

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);

	FXAAShader.BindProgram();
	FXAAShader.BindSampler("uImage", 0, PostColorBuffer0);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostGamma(const SceneView& sceneView)
{
	float time = (float)TimeSystem::GetTimerElapsedTime(sceneView.WorldData, eEngineTimer::GAMEPLAY);

	const ScreenSize screenSize = RDI->GetScreenSize();

	float grainScale = 0.01f;
	float vignetteScale = 0.1f;
	float abberationScale = 0.1f;
	Color tint = Color::WHITE;
	float gamma = 2.2f;
	const PostprocessSettingsComponent* postCmp = sceneView.CameraCmp->GetSibling<PostprocessSettingsComponent>();
	if (postCmp) {
		grainScale = postCmp->GrainScale;
		vignetteScale = postCmp->VignetteScale;
		abberationScale = postCmp->AbberationScale;
		gamma = postCmp->Gamma;
		tint = postCmp->Tint;
	}

	GammaShader.BindProgram();
	GammaShader.BindSampler("uImage", 0, PostColorBuffer1);
	GammaShader.BindSampler("uSplashImage", 1, Splash->GetTextureProxy()->GetResourceID());
	GammaShader.SetUniform("uSplashTint", Color(1.0f, 0.0f, 0.0f, 1.0f) * 0.8f);
	GammaShader.SetUniform("uTime", time);
	GammaShader.SetUniform("uRes", Vector((float)screenSize.Width, (float)screenSize.Height, 1.0f / screenSize.Width, 1.0f / screenSize.Height));
	GammaShader.SetUniform("uTint", tint);
	GammaShader.SetUniform("uGrainScale", grainScale);
	GammaShader.SetUniform("uVignetteScale", vignetteScale);
	GammaShader.SetUniform("uAbberationScale", abberationScale);
	GammaShader.SetUniform("uGamma", gamma);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void TiledForwardRenderer::EditorDebug(const SceneView& sceneView)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	const Matrix& clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();
	EditorDebugShader.BindProgram();
	EditorDebugShader.SetUniform("uMVP", clipFromWorld);

	// Render Lines
	{
		auto debugLinesComponent = sceneView.WorldData->GetWorldComponent<DebugDrawStateWorldComponent>();
		auto& debugLines = debugLinesComponent->DebugLines;
		auto& debugLinesColors = debugLinesComponent->DebugLinesColors;

		DebugRenderingBuffers debugLinesBuffers; // vertex and color buffer

												 // set up buffer
		glBindVertexArray(debugLinesBuffers.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, debugLinesBuffers.VBO);
		glBufferData(GL_ARRAY_BUFFER, debugLines.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLine)
			+ debugLinesColors.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLineColor), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, debugLines.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLine), (GLvoid*)debugLines.GetData());
		glBufferSubData(GL_ARRAY_BUFFER, debugLines.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLine), debugLinesColors.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLineColor), (GLvoid*)debugLinesColors.GetData());
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), NULL);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Color), (GLvoid*)(debugLines.GetSize() * sizeof(DebugDrawStateWorldComponent::DebugLine)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_LINES, 0, (GLsizei)debugLines.GetSize() * 2);
		glBindVertexArray(0);

		debugLines.Clear();
		debugLinesColors.Clear();
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
	Matrix ortho;
	ortho.SetOrthographic(
		sceneView.Rect.GetMin().Y * screen.Height,
		sceneView.Rect.GetMax().Y * screen.Height,
		sceneView.Rect.GetMin().X * screen.Width,
		sceneView.Rect.GetMax().X * screen.Width,
		-1,
		 1
	);
	Text2DShader.BindProgram();
	Text2DShader.SetUniform("u_projection", ortho);

	for (auto componentsTuple : sceneView.WorldData->IterateComponents<ScreenSpaceTextComponent>())
	{
		ScreenSpaceTextComponent* textCmp = std::get<ScreenSpaceTextComponent*>(componentsTuple);
		Text2D& text = textCmp->GetText();
		Text2DShader.SetUniform("u_textColor", text.GetFontColor());
		Text2DShader.SetUniform("u_position", Vector((float)textCmp->GetScreenPosition().X, (float)textCmp->GetScreenPosition().Y, 0));
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
		light.Position = Vector::ZERO;
		light.Color = Vector::ZERO;
		light.RangeIntensity = Vector(-1.0f, 0.0f, 0.0f);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::UpdateLightsBufferFromScene(const SceneView& sceneView)
{
	Dynarray<Vector> positions;
	Dynarray<Vector> color;
	Dynarray<Vector> rangeIntensity;

	int lightCounter = 0;
	for (const PointLightComponent* pointLightCmp : sceneView.PointLights)
	{
		const EntityTransform& transform = pointLightCmp->GetTransform();

		positions.PushBack(transform.GetGlobalTranslation());
		color.PushBack(Vector(pointLightCmp->GetColor()));
		rangeIntensity.PushBack(Vector(pointLightCmp->GetRange(), pointLightCmp->GetIntensity(), 0.0f));

		++lightCounter;

		if (lightCounter > MAX_NUM_LIGHTS)
		{
			gConsole.LogInfo("TiledForwardRenderer::UpdateLightsBufferFromScene more lights than supported by renderer({})", MAX_NUM_LIGHTS);
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

		// 		gConsole.LogInfo("TiledForwardRenderer::UpdateLightsBufferFromScene set Position: {}, Radius: {}",
		// 			light.Position, light.Radius);
	}

	int remainingSlots = MAX_NUM_LIGHTS - lightCounter;
	for (int i = 0; i < remainingSlots; ++i)
	{
		Light &light = lights[lightCounter + i];
		light.Position = Vector::ZERO;
		light.Color = Vector::ZERO;
		light.RangeIntensity = Vector(-1.0f, 0.0f, 0.0f);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void TiledForwardRenderer::DebugDepthPrepass(const SceneView& sceneView)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Weirdly, moving this call drops performance into the floor
	DebugQuadDepthPrepassShader.BindProgram();
	DebugQuadDepthPrepassShader.SetUniform("uNear", sceneView.CameraCmp->GetClippingPlaneNear());
	DebugQuadDepthPrepassShader.SetUniform("uFar", sceneView.CameraCmp->GetClippingPlaneFar());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PreDepthBuffer);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void TiledForwardRenderer::DebugLightAccum(const SceneView& sceneView)
{
	float time = (float)(sceneView.WorldData->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DebugLightAccumShader.BindProgram();
	DebugLightAccumShader.SetUniform("uTime", time);
	DebugLightAccumShader.SetUniform("uWorkGroupsX", (int)WorkGroupsX);
	DebugLightAccumShader.SetUniform("uWorkGroupsY", (int)WorkGroupsY);
	DebugLightAccumShader.SetUniform("uLightCount", (int)std::min((int)sceneView.PointLights.GetSize(), MAX_NUM_LIGHTS));

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, LightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, VisibleLightIndicesBuffer);

	const Matrix& clipFromWorld = sceneView.CameraCmp->GetClipFromWorld();
	for (auto componentsTuple : sceneView.WorldData->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& worldFromModel = transform.GetWorldFromModel();
		DebugLightAccumShader.SetUniform("uClipFromModel", clipFromWorld * worldFromModel);
		DebugLightAccumShader.SetUniform("uWorldFromModel", worldFromModel);

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