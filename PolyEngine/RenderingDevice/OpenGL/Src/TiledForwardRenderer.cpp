#include "TiledForwardRenderer.hpp"

#include <math.h>

#include "GLRenderingDevice.hpp"
#include "Proxy/GLMeshDeviceProxy.hpp"
#include "Pipeline/RenderingPassBase.hpp"
#include "Common/GLShaderProgram.hpp"

#include <ECS/World.hpp>
#include <Math/BasicMath.hpp>
#include <Math/AARect.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/Lighting/LightSourceComponent.hpp>
#include <Rendering/MeshRenderingComponent.hpp>
#include <Time/TimeWorldComponent.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "Common/stb_image.hpp"

using namespace Poly;

TiledForwardRenderer::TiledForwardRenderer(GLRenderingDevice* rdi)
	: IRendererInterface(rdi),
	depthShader("Shaders/depth.vert.glsl", "Shaders/depth.frag.glsl"),
	lightCullingShader("Shaders/light_culling.comp.glsl"),
	debugQuadDepthPrepassShader("Shaders/debugQuadDepthPrepass.vert.glsl", "Shaders/debugQuadDepthPrepass.frag.glsl"),
	debugLightAccumShader("Shaders/debugLightAccum.vert.glsl", "Shaders/debugLightAccum.frag.glsl"),
	LightAccumulationShader("Shaders/lightAccumulation.vert.glsl", "Shaders/lightAccumulation.frag.glsl"),
	hdrShader("Shaders/hdr.vert.glsl", "Shaders/hdr.frag.glsl"),
	SSAOShader("Shaders/hdr.vert.glsl", "Shaders/ssao.frag.glsl"),
	GammaShader("Shaders/hdr.vert.glsl", "Shaders/gamma.frag.glsl"),
	LinearizeDepthShader("Shaders/hdr.vert.glsl", "Shaders/linearizeDepth.frag.glsl"),
	integrateBRDFShader("Shaders/hdr.vert.glsl", "Shaders/integrateBRDF.frag.glsl"),
	ParticleShader("Shaders/instanced.vert.glsl", "Shaders/instanced.frag.glsl"),
	TranslucentShader("Shaders/transparent.vert.glsl", "Shaders/transparent.frag.glsl"),
	SkyboxShader("Shaders/skybox.vert.glsl", "Shaders/skybox.frag.glsl"),
	equiToCubemapShader("Shaders/equiHdr.vert.glsl", "Shaders/equiHdr.frag.glsl"),
	equirectangularToCubemapShader("Shaders/equiToCubemap.vert.glsl", "Shaders/equiToCubemap.frag.glsl"),
	cubemapIrradianceShader("Shaders/cubemapIrradiance.vert.glsl", "Shaders/cubemapIrradiance.frag.glsl"),
	prefilterCubemapShader("Shaders/prefilterCubemap.vert.glsl", "Shaders/prefilterCubemap.frag.glsl"),
	text2DShader("Shaders/text2D.vert.glsl", "Shaders/text2D.frag.glsl")
{
	LightAccumulationShader.RegisterUniform("vec4", "uViewPosition");
	LightAccumulationShader.RegisterUniform("mat4", "uClipFromModel");
	LightAccumulationShader.RegisterUniform("mat4", "uWorldFromModel");
	LightAccumulationShader.RegisterUniform("vec4", "uMaterial.Emissive");
	LightAccumulationShader.RegisterUniform("vec4", "uMaterial.Albedo");
	LightAccumulationShader.RegisterUniform("float", "uMaterial.Roughness");
	LightAccumulationShader.RegisterUniform("float", "uMaterial.Metallic");
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

	SkyboxShader.RegisterUniform("mat4", "uClipFromWorld");
	SkyboxShader.RegisterUniform("float", "uTime");

	TranslucentShader.RegisterUniform("vec4", "uViewPosition");
	TranslucentShader.RegisterUniform("mat4", "uClipFromModel");
	TranslucentShader.RegisterUniform("mat4", "uWorldFromModel");
	TranslucentShader.RegisterUniform("vec4", "uMaterial.Ambient");
	TranslucentShader.RegisterUniform("vec4", "uMaterial.Diffuse");
	TranslucentShader.RegisterUniform("vec4", "uMaterial.Specular");
	TranslucentShader.RegisterUniform("float", "uMaterial.Shininess");
	for (int i = 0; i < 8; ++i)
	{
		String baseName = String("uDirectionalLight[") + String::From(i) + String("].");
		TranslucentShader.RegisterUniform("vec4", baseName + "ColorIntensity");
		TranslucentShader.RegisterUniform("vec4", baseName + "Direction");
	}
	TranslucentShader.RegisterUniform("int", "uDirectionalLightCount");
	
	ParticleShader.RegisterUniform("float", "uTime");
	ParticleShader.RegisterUniform("mat4", "uScreenFromView");
	ParticleShader.RegisterUniform("mat4", "uViewFromWorld");
	ParticleShader.RegisterUniform("mat4", "uWorldFromModel");
	ParticleShader.RegisterUniform("vec4", "uColor");

	LinearizeDepthShader.RegisterUniform("float", "uNear");
	LinearizeDepthShader.RegisterUniform("float", "uFar");
}

void TiledForwardRenderer::Init()
{
	gConsole.LogInfo("TiledForwardRenderer::Init");
	
	const ScreenSize screenSize = RDI->GetScreenSize();

	gConsole.LogInfo("TiledForwardRenderer::Init SCREEN_SIZE: ({},{})", screenSize.Width, screenSize.Height);

	LoadHDR();

	CaptureCubemap();

	CaptureDifuseIrradiance();

	CaptureSpecularPrefilteredMap();

	CapturePreintegratedBRDF();

	CreateRenderTargets(screenSize);

	CreateLightBuffers(screenSize);

	SetupLightsBufferFromScene();

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void TiledForwardRenderer::LoadHDR()
{
	String absoluteHDRPath = gAssetsPathConfig.GetAssetsPath(eResourceSource::GAME) + "HDR/HDR.hdr";
	gConsole.LogInfo("TiledForwardRenderer::Init absoluteHDRPath: {}", absoluteHDRPath);
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf(absoluteHDRPath.GetCStr(), &width, &height, &nrComponents, 0);
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		gConsole.LogInfo("TiledForwardRenderer::Init Failed to load HDR image.");
	}
}

void TiledForwardRenderer::CaptureCubemap()
{
	// create cubemap resource for capture
	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap create cubemap resource");
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (GLuint i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap cubemap resource created");

	// Create FBO for HDR capture
	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap Create capture FBO");
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap Capture FBO created");

	// Projection
	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap create matrices");
	Matrix uClipFromView;
	uClipFromView.SetPerspective(90.0_deg, 1.0f, 0.1f, 10.0f);

	Matrix ViewFromModel[] =
	{
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector( 1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(-1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector( 0.0f, -1.0f,  0.0f), Vector(0.0f,  0.0f, -1.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector( 0.0f,  1.0f,  0.0f), Vector(0.0f,  0.0f,  1.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector( 0.0f,  0.0f,  1.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector( 0.0f,  0.0f, -1.0f), Vector(0.0f, -1.0f,  0.0f))
	};
	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap matrices created");
	
	// convert HDR equirectangular environment map to cubemap equivalent
	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap start cubemap rendering loop");
	equirectangularToCubemapShader.BindProgram();
	equirectangularToCubemapShader.SetUniform("uEquirectangularMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	 
	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap cubemap rendering: {}", i);
		equirectangularToCubemapShader.SetUniform("uClipFromModel", uClipFromView * ViewFromModel[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		 
		// renderCube(); // renders a 1x1 cube
		glBindVertexArray(RDI->PrimitivesCube->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap cubemap rendered");

	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap generate env cubemap mipmaps");
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	gConsole.LogInfo("TiledForwardRenderer::CaptureCubemap env cubemap mipmaps generated");
}

void TiledForwardRenderer::CaptureDifuseIrradiance()
{
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
			GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Create FBO for HDR capture
	gConsole.LogInfo("TiledForwardRenderer::CaptureIrradiance Create capture FBO");
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	gConsole.LogInfo("TiledForwardRenderer::CaptureIrradiance Capture FBO created");

	// Projection
	gConsole.LogInfo("TiledForwardRenderer::CaptureIrradiance create matrices");
	Matrix uClipFromView;
	uClipFromView.SetPerspective(90.0_deg, 1.0f, 0.1f, 10.0f);

	Matrix ViewFromModel[] =
	{
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(-1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, -1.0f,  0.0f), Vector(0.0f,  0.0f, -1.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(0.0f,  1.0f,  0.0f), Vector(0.0f,  0.0f,  1.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(0.0f,  0.0f,  1.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(0.0f,  0.0f, -1.0f), Vector(0.0f, -1.0f,  0.0f))
	};
	gConsole.LogInfo("TiledForwardRenderer::CaptureIrradiance matrices created");

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	cubemapIrradianceShader.BindProgram();
	cubemapIrradianceShader.SetUniform("uEnvCubemap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		cubemapIrradianceShader.SetUniform("uClipFromModel", uClipFromView * ViewFromModel[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(RDI->PrimitivesCube->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::CaptureSpecularPrefilteredMap()
{
	gConsole.LogInfo("TiledForwardRenderer::CaptureSpecularPrefilteredMap create cubemap resource");
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		// glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	gConsole.LogInfo("TiledForwardRenderer::CaptureSpecularPrefilteredMap cubemap resource created");

	// Create FBO for HDR capture
	gConsole.LogInfo("TiledForwardRenderer::CaptureSpecularPrefilteredMap Create capture FBO");
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	gConsole.LogInfo("TiledForwardRenderer::CaptureSpecularPrefilteredMap Capture FBO created");

	// Projection
	gConsole.LogInfo("TiledForwardRenderer::CaptureSpecularPrefilteredMap create matrices");
	Matrix uClipFromView;
	uClipFromView.SetPerspective(90.0_deg, 1.0f, 0.1f, 10.0f);

	Matrix ViewFromModel[] =
	{
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(-1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, -1.0f,  0.0f), Vector(0.0f,  0.0f, -1.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(0.0f,  1.0f,  0.0f), Vector(0.0f,  0.0f,  1.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(0.0f,  0.0f,  1.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector(0.0f, 0.0f, 0.0f), Vector(0.0f,  0.0f, -1.0f), Vector(0.0f, -1.0f,  0.0f))
	};
	gConsole.LogInfo("TiledForwardRenderer::CaptureSpecularPrefilteredMap matrices created");

	gConsole.LogInfo("TiledForwardRenderer::CaptureSpecularPrefilteredMap capture prefiltered cubemap");
	prefilterCubemapShader.BindProgram();
	prefilterCubemapShader.SetUniform("uEnvironmentMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		// unsigned int mipWidth = 128 * std::pow(0.5, mip);
		// unsigned int mipHeight = 128 * std::pow(0.5, mip);
		unsigned int mipWidth = 512 * std::pow(0.5, mip);
		unsigned int mipHeight = 512 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterCubemapShader.SetUniform("uRoughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterCubemapShader.SetUniform("uClipFromModel", uClipFromView * ViewFromModel[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glBindVertexArray(RDI->PrimitivesCube->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	gConsole.LogInfo("TiledForwardRenderer::CaptureSpecularPrefilteredMap prefiltered cubemap captured");
}

void TiledForwardRenderer::CapturePreintegratedBRDF()
{
	// Create texture resource
	gConsole.LogInfo("TiledForwardRenderer::PreintegrateBRDF Create texture resource");
	glGenTextures(1, &preintegratedBrdfLUT);
	glBindTexture(GL_TEXTURE_2D, preintegratedBrdfLUT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	CHECK_GL_ERR();

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
	integrateBRDFShader.BindProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, preintegratedBrdfLUT, 0);	
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
	GLuint SCREEN_SIZE_X = size.Width;
	GLuint SCREEN_SIZE_Y = size.Height;

	workGroupsX = (SCREEN_SIZE_X + (SCREEN_SIZE_X % 16)) / 16;
	workGroupsY = (SCREEN_SIZE_Y + (SCREEN_SIZE_Y % 16)) / 16;
	size_t numberOfTiles = workGroupsX * workGroupsY;
	gConsole.LogInfo("TiledForwardRenderer::Init workGroups: ({},{}), numberOfTiles: {}", workGroupsX, workGroupsY, numberOfTiles);

	// Generate our shader storage buffers
	glGenBuffers(1, &lightBuffer);
	glGenBuffers(1, &visibleLightIndicesBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Light) * MAX_NUM_LIGHTS, 0, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, visibleLightIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(VisibleIndex) * MAX_NUM_LIGHTS * numberOfTiles, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	CHECK_GL_ERR();
}

void TiledForwardRenderer::DeleteLightBuffers()
{
	if(lightBuffer > 0)
		glDeleteBuffers(1, &lightBuffer);

	if(visibleLightIndicesBuffer > 0)
		glDeleteBuffers(1, &visibleLightIndicesBuffer);
}

void TiledForwardRenderer::CreateRenderTargets(const ScreenSize& size)
{
	GLuint SCREEN_SIZE_X = size.Width;
	GLuint SCREEN_SIZE_Y = size.Height;

	glGenFramebuffers(1, &FBOdepthMap);
	glGenTextures(1, &preDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, preDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOdepthMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, preDepthBuffer, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK_GL_ERR();
	CHECK_FBO_STATUS();

	// Create a floating point HDR frame buffer and a floating point color buffer (as a texture)
	glGenFramebuffers(1, &FBOhdr);

	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &normalBuffer);
	glBindTexture(GL_TEXTURE_2D, normalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// It will also need a depth component as a render buffer, attached to the hdrFBO
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_SIZE_X, SCREEN_SIZE_Y);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK_GL_ERR();
	CHECK_FBO_STATUS();

	// Create pair of frame buffers for post process to swap
	glGenFramebuffers(1, &FBOpost0);

	glGenTextures(1, &postColorBuffer0);
	glBindTexture(GL_TEXTURE_2D, postColorBuffer0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postColorBuffer0, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK_GL_ERR();
	CHECK_FBO_STATUS();

	glGenFramebuffers(1, &FBOpost1);

	glGenTextures(1, &postColorBuffer1);
	glBindTexture(GL_TEXTURE_2D, postColorBuffer1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postColorBuffer1, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &linearDepth);
	glBindTexture(GL_TEXTURE_2D, linearDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	CHECK_GL_ERR();
	CHECK_FBO_STATUS();
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

	DeleteLightBuffers();

	DeleteRenderTargets();
}

void TiledForwardRenderer::DeleteRenderTargets()
{
	if (preDepthBuffer > 0)
		glDeleteTextures(1, &preDepthBuffer);

	if (FBOdepthMap>0)
		glDeleteFramebuffers(1, &FBOdepthMap);

	if (colorBuffer > 0)
		glDeleteTextures(1, &colorBuffer);

	if (normalBuffer > 0)
		glDeleteTextures(1, &normalBuffer);

	if (rboDepth > 0)
		glDeleteRenderbuffers(1, &rboDepth);

	if (FBOhdr > 0)
		glDeleteFramebuffers(1, &FBOhdr);

	if (postColorBuffer0 > 0)
		glDeleteTextures(1, &postColorBuffer0);

	if (FBOpost0 > 0)
		glDeleteFramebuffers(1, &FBOpost0);

	if (postColorBuffer1 > 0)
		glDeleteTextures(1, &postColorBuffer1);

	if (linearDepth > 0)
		glDeleteTextures(1, &linearDepth);

	if (FBOpost1 > 0)
		glDeleteFramebuffers(1, &FBOpost1);

	CHECK_GL_ERR();
	CHECK_FBO_STATUS();
}

void TiledForwardRenderer::Render(const SceneView& sceneView)
{
	// gConsole.LogInfo("TiledForwardRenderer::Render");

	UpdateLightsBufferFromScene(sceneView);

	RenderDepthPrePass(sceneView);

	ComputeLightCulling(sceneView);

	RenderOpaqueLit(sceneView);

	RenderSkybox(sceneView);

	// RenderEquiCube(sceneView);

	// RenderTranslucentLit(sceneView);

	// RenderParticleUnlit(sceneView.world, sceneView.cameraCmp);

	LinearizeDepth(sceneView);

	PostTonemapper(sceneView);

	// PostSSAO(cameraCmp);

	UIText2D(sceneView);

	PostGamma();
}

void TiledForwardRenderer::RenderEquiCube(const SceneView& sceneView)
{
		const Matrix ClipFromWorld = sceneView.cameraCmp->GetClipFromWorld();

		equiToCubemapShader.BindProgram();
		equiToCubemapShader.SetUniform("uClipFromModel", ClipFromWorld);

		// GLuint CubemapID = static_cast<const GLCubemapDeviceProxy*>(SkyboxWorldCmp->GetCubemap().GetTextureProxy())->GetTextureID();

		glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

		glBindFragDataLocation(equiToCubemapShader.GetProgramHandle(), 0, "color");
		glBindFragDataLocation(equiToCubemapShader.GetProgramHandle(), 1, "normal");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		equiToCubemapShader.SetUniform("uEquirectangularMap", 0);

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
	depthShader.BindProgram();

	glBindFramebuffer(GL_FRAMEBUFFER, FBOdepthMap);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	const Matrix& ClipFromWorld = sceneView.cameraCmp->GetClipFromWorld();
	
	for (const MeshRenderingComponent* meshCmp : sceneView.OpaqueQueue)
	{
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		depthShader.SetUniform("uScreenFromModel", ClipFromWorld * WorldFromModel);
		
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::ComputeLightCulling(const SceneView& sceneView)
{
	float Time = (float)(sceneView.world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());

	// gConsole.LogInfo("TiledForwardRenderer::LightCulling Time: {}, workGroupsX: {}, workGroupsY: {}",
	// 	Time, workGroupsX, workGroupsY
	// );

	lightCullingShader.BindProgram();
	lightCullingShader.SetUniform("uViewFromWorld", sceneView.cameraCmp->GetViewFromWorld());
	lightCullingShader.SetUniform("uClipFromWorld", sceneView.cameraCmp->GetClipFromWorld());
	lightCullingShader.SetUniform("uClipFromView",  sceneView.cameraCmp->GetClipFromView());
	lightCullingShader.SetUniform("uScreenSizeX", RDI->GetScreenSize().Width);
	lightCullingShader.SetUniform("uScreenSizeY", RDI->GetScreenSize().Height);
	lightCullingShader.SetUniform("uWorkGroupsX", (int)workGroupsX);
	lightCullingShader.SetUniform("uWorkGroupsY", (int)workGroupsY);
	lightCullingShader.SetUniform("uLightCount", (int)std::min(sceneView.PointLights.GetSize(), MAX_NUM_LIGHTS));

	// Bind depth map texture to texture location 4 (which will not be used by any model texture)
	glActiveTexture(GL_TEXTURE4);
	glUniform1i(glGetUniformLocation(lightCullingShader.GetProgramHandle(), "uDepthMap"), 4);
	glBindTexture(GL_TEXTURE_2D, preDepthBuffer);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

	glDispatchCompute(workGroupsX, workGroupsY, 1);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TiledForwardRenderer::RenderOpaqueLit(const SceneView& sceneView)
{
	// gConsole.LogInfo("TiledForwardRenderer::AccumulateLights");

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	const EntityTransform& cameraTransform = sceneView.cameraCmp->GetTransform();

	LightAccumulationShader.BindProgram();
	LightAccumulationShader.SetUniform("uLightCount", (int)std::min(sceneView.PointLights.GetSize(), MAX_NUM_LIGHTS));
	LightAccumulationShader.SetUniform("uWorkGroupsX", (int)workGroupsX);
	LightAccumulationShader.SetUniform("uWorkGroupsY", (int)workGroupsY);
	LightAccumulationShader.SetUniform("uViewPosition", cameraTransform.GetGlobalTranslation());

	glBindFragDataLocation(LightAccumulationShader.GetProgramHandle(), 0, "oColor");
	glBindFragDataLocation(LightAccumulationShader.GetProgramHandle(), 1, "oNormal");

	static const int MAX_LIGHT_COUNT_DIRECTIONAL = 8;
	int dirLightsCount = 0;
	for ( const DirectionalLightComponent* dirLightCmp : sceneView.DirectionalLights)
	{		
		const EntityTransform& transform = dirLightCmp->GetTransform();
		String baseName = String("uDirectionalLight[") + String::From(dirLightsCount) + String("].");
		Color ColorIntensity = dirLightCmp->GetColor();
		ColorIntensity.A = dirLightCmp->GetIntensity();
		LightAccumulationShader.SetUniform(baseName + "ColorIntensity", ColorIntensity);
		LightAccumulationShader.SetUniform(baseName + "Direction", MovementSystem::GetGlobalForward(transform));

		++dirLightsCount;
		if (dirLightsCount == MAX_LIGHT_COUNT_DIRECTIONAL)
			break;
	}
	LightAccumulationShader.SetUniform("uDirectionalLightCount", dirLightsCount);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

	const Matrix& ClipFromWorld = sceneView.cameraCmp->GetClipFromWorld();
	for (const MeshRenderingComponent* meshCmp : sceneView.OpaqueQueue)
	{
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		LightAccumulationShader.SetUniform("uClipFromModel", ClipFromWorld * WorldFromModel);
		LightAccumulationShader.SetUniform("uWorldFromModel", WorldFromModel);
		
		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			if (meshCmp->GetShadingModel() == eShadingModel::PBR)
			{
				// PhongMaterial material = meshCmp->GetMaterial(i);
				// lightAccumulationShader.SetUniform("uMaterial.Ambient", material.AmbientColor);
				// lightAccumulationShader.SetUniform("uMaterial.Diffuse", material.DiffuseColor);
				// lightAccumulationShader.SetUniform("uMaterial.Specular", material.SpecularColor);
				// lightAccumulationShader.SetUniform("uMaterial.Shininess", material.Shininess);

				PBRMaterial material = meshCmp->GetPBRMaterial(i);
				LightAccumulationShader.SetUniform("uMaterial.Emissive", material.Emissive);
				LightAccumulationShader.SetUniform("uMaterial.Albedo", material.Albedo);
				LightAccumulationShader.SetUniform("uMaterial.Roughness", material.Roughness);
				LightAccumulationShader.SetUniform("uMaterial.Metallic", material.Metallic);

				const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
				glBindVertexArray(meshProxy->GetVAO());

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
				LightAccumulationShader.SetUniform("uIrradianceMap", 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
				LightAccumulationShader.SetUniform("uPrefilterMap", 1);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, preintegratedBrdfLUT);
				LightAccumulationShader.SetUniform("uBrdfLUT", 2);

				const TextureResource* EmissiveTexture = subMesh->GetMeshData().GetEmissiveMap();
				GLuint EmissiveID = EmissiveTexture == nullptr
					? RDI->FallbackBlackTexture
					: static_cast<const GLTextureDeviceProxy*>(EmissiveTexture->GetTextureProxy())->GetTextureID();
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, EmissiveID);
				LightAccumulationShader.SetUniform("uEmissiveMap", 3);

				const TextureResource* AlbedoTexture = subMesh->GetMeshData().GetAlbedoMap();
				GLuint AlbedoID = AlbedoTexture == nullptr
					? RDI->FallbackWhiteTexture
					: static_cast<const GLTextureDeviceProxy*>(AlbedoTexture->GetTextureProxy())->GetTextureID();
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, AlbedoID);
				LightAccumulationShader.SetUniform("uAlbedoMap", 4);
				
				const TextureResource* NormalMap = subMesh->GetMeshData().GetNormalMap();
				GLuint NormalMapID = NormalMap == nullptr
					? RDI->FallbackNormalMap
					: static_cast<const GLTextureDeviceProxy*>(NormalMap->GetTextureProxy())->GetTextureID();
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, NormalMapID);
				LightAccumulationShader.SetUniform("uNormalMap", 5);
				 
				const TextureResource* RoughnessTexture = subMesh->GetMeshData().GetRoughnessMap();
				GLuint RoughnessID = RoughnessTexture == nullptr
					? RDI->FallbackBlackTexture
					: static_cast<const GLTextureDeviceProxy*>(RoughnessTexture->GetTextureProxy())->GetTextureID();
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, RoughnessID);
				LightAccumulationShader.SetUniform("uRoughnessMap", 6);
				
				const TextureResource* MetallicTexture = subMesh->GetMeshData().GetMetallicMap();
				GLuint MetallicID = MetallicTexture == nullptr
					? RDI->FallbackBlackTexture
					: static_cast<const GLTextureDeviceProxy*>(MetallicTexture->GetTextureProxy())->GetTextureID();
				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, MetallicID);
				LightAccumulationShader.SetUniform("uMetallicMap", 7);
				
				const TextureResource* AmbientOcclusionTexture = subMesh->GetMeshData().GetAmbientOcclusionMap();
				GLuint AmbientOcclusionID = AmbientOcclusionTexture == nullptr
					? RDI->FallbackWhiteTexture
					: static_cast<const GLTextureDeviceProxy*>(AmbientOcclusionTexture->GetTextureProxy())->GetTextureID();
				glActiveTexture(GL_TEXTURE8);
				glBindTexture(GL_TEXTURE_2D, AmbientOcclusionID);
				LightAccumulationShader.SetUniform("uAmbientOcclusionMap", 8);

				glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
				++i;
			}
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::RenderSkybox(const SceneView& sceneView)
{
	float Time = (float)(sceneView.world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());

	// const SkyboxWorldComponent* SkyboxWorldCmp = world->GetWorldComponent<SkyboxWorldComponent>();
	// if (SkyboxWorldCmp != nullptr)
	// {
		const Matrix ClipFromView = sceneView.cameraCmp->GetClipFromView();
		Matrix ViewFromWorld = sceneView.cameraCmp->GetViewFromWorld();
		// center cube in view space by setting translation to 0 for x, y and z.
		// SetTranslation resets Matrix to identity
		ViewFromWorld.Data[3] = 0.0f;
		ViewFromWorld.Data[7] = 0.0f;
		ViewFromWorld.Data[11] = 0.0f;

		Matrix ClipFromWorld = ClipFromView * ViewFromWorld;

		SkyboxShader.BindProgram();
		SkyboxShader.SetUniform("uClipFromWorld", ClipFromWorld);
		// SkyboxShader.SetUniform("uTime", Time);

		// GLuint CubemapID = static_cast<const GLCubemapDeviceProxy*>(SkyboxWorldCmp->GetCubemap().GetTextureProxy())->GetTextureID();

		glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);
		
		glBindFragDataLocation(LightAccumulationShader.GetProgramHandle(), 0, "color");
		glBindFragDataLocation(LightAccumulationShader.GetProgramHandle(), 1, "normal");
		
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);

		glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapID);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glBindVertexArray(RDI->PrimitivesCube->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// }
}

void TiledForwardRenderer::RenderTranslucentLit(const SceneView& sceneView)
{
	// gConsole.LogInfo("TiledForwardRenderer::RenderTranslucenLit");
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	// glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const EntityTransform& cameraTransform = sceneView.cameraCmp->GetTransform();
	TranslucentShader.BindProgram();
	TranslucentShader.SetUniform("uViewPosition", cameraTransform.GetGlobalTranslation());

	static const int MAX_LIGHT_COUNT_DIRECTIONAL = 8;
	int dirLightsCount = 0;
	for (const DirectionalLightComponent* dirLightCmp: sceneView.DirectionalLights)
	{
		const EntityTransform& transform = dirLightCmp->GetTransform();
		String baseName = String("uDirectionalLight[") + String::From(dirLightsCount) + String("].");
		Color ColorIntensity = dirLightCmp->GetColor();
		ColorIntensity.A = dirLightCmp->GetIntensity();
		TranslucentShader.SetUniform(baseName + "ColorIntensity", ColorIntensity);
		TranslucentShader.SetUniform(baseName + "Direction", MovementSystem::GetGlobalForward(transform));

		++dirLightsCount;
		if (dirLightsCount == MAX_LIGHT_COUNT_DIRECTIONAL)
			break;
	}
	TranslucentShader.SetUniform("uDirectionalLightCount", dirLightsCount);

	glBindFragDataLocation(TranslucentShader.GetProgramHandle(), 0, "color");
	glBindFragDataLocation(TranslucentShader.GetProgramHandle(), 1, "normal");
	// glBindFragDataLocation(TranslucentShader.GetProgramHandle(), 0, "oColor");
	// glBindFragDataLocation(TranslucentShader.GetProgramHandle(), 1, "oNormal");

	const Matrix& ClipFromWorld = sceneView.cameraCmp->GetClipFromWorld();
	for (const MeshRenderingComponent* meshCmp : sceneView.TranslucentQueue)
	{
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		TranslucentShader.SetUniform("uClipFromModel", ClipFromWorld * WorldFromModel);
		TranslucentShader.SetUniform("uWorldFromModel", WorldFromModel);

		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			PhongMaterial material = meshCmp->GetMaterial(i);
			TranslucentShader.SetUniform("uMaterial.Ambient", material.AmbientColor);
			TranslucentShader.SetUniform("uMaterial.Diffuse", material.DiffuseColor);
			TranslucentShader.SetUniform("uMaterial.Specular", material.SpecularColor);
			TranslucentShader.SetUniform("uMaterial.Shininess", material.Shininess);

			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
			glBindVertexArray(meshProxy->GetVAO());

			const TextureResource* DiffuseTexture = subMesh->GetMeshData().GetAlbedoMap();
			GLuint DiffuseID = DiffuseTexture == nullptr
				? RDI->FallbackWhiteTexture
				: static_cast<const GLTextureDeviceProxy*>(DiffuseTexture->GetTextureProxy())->GetTextureID();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, DiffuseID);
			TranslucentShader.SetUniform("uAlbedoMap", 0);

			const TextureResource* NormalMap = subMesh->GetMeshData().GetNormalMap();
			GLuint NormalMapID = NormalMap == nullptr
				? RDI->FallbackNormalMap
				: static_cast<const GLTextureDeviceProxy*>(NormalMap->GetTextureProxy())->GetTextureID();
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, NormalMapID);
			TranslucentShader.SetUniform("uNormalMap", 1);

			const TextureResource* SpecularTexture = subMesh->GetMeshData().GetRoughnessMap();
			GLuint SpecularID = SpecularTexture == nullptr
				? RDI->FallbackWhiteTexture
				: static_cast<const GLTextureDeviceProxy*>(SpecularTexture->GetTextureProxy())->GetTextureID();
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, SpecularID);
			TranslucentShader.SetUniform("uSpecularMap", 2);

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			++i;
		}
	}
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);

	// glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::RenderParticleUnlit(World* world, const CameraComponent* cameraCmp)
{
	// gConsole.LogInfo("TiledForwardRenderer::RenderParticleUnlit");

	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	const Matrix& ViewFromWorld = cameraCmp->GetViewFromWorld();
	const Matrix& ScreenFromView = cameraCmp->GetClipFromView();

	glBindFramebuffer(GL_FRAMEBUFFER, FBOhdr);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	// TODO test these blending options
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);

	ParticleShader.BindProgram();
	ParticleShader.SetUniform("uTime", Time);
	ParticleShader.SetUniform("uScreenFromView", ScreenFromView);

	glBindFragDataLocation(TranslucentShader.GetProgramHandle(), 0, "color");
	glBindFragDataLocation(TranslucentShader.GetProgramHandle(), 1, "normal");
	// glBindFragDataLocation(TranslucentShader.GetProgramHandle(), 0, "oColor");
	// glBindFragDataLocation(TranslucentShader.GetProgramHandle(), 1, "oNormal");

	for (auto componentsTuple : world->IterateComponents<ParticleComponent>())
	{
		const ParticleComponent* particleCmp = std::get<ParticleComponent*>(componentsTuple);
		const EntityTransform& transform = particleCmp->GetTransform();
		const Matrix& WorldFromModel = particleCmp->GetEmitter()->GetSettings().SimulationSpace == ParticleEmitter::eSimulationSpace::LOCAL_SPACE
			? transform.GetWorldFromModel()
			: Matrix();

		ParticleShader.SetUniform("uViewFromWorld", ViewFromWorld);
		ParticleShader.SetUniform("uWorldFromModel", WorldFromModel);

		ParticleEmitter::Settings emitterSettings = particleCmp->GetEmitter()->GetSettings();
		ParticleShader.SetUniform("uEmitterColor", emitterSettings.BaseColor);

		SpritesheetSettings spriteSettings = emitterSettings.SpritesheetSettings;
		ParticleShader.SetUniform("uSpriteColor", spriteSettings.SpriteColor);
		float startFrame = spriteSettings.IsRandomStartFrame ? RandomRange(0.0f, spriteSettings.SubImages.X * spriteSettings.SubImages.Y) : spriteSettings.StartFrame;
		ParticleShader.SetUniform("uSpriteStartFrame", startFrame);
		ParticleShader.SetUniform("uSpriteSpeed", spriteSettings.Speed);
		ParticleShader.SetUniform("uSpriteSubImages", spriteSettings.SubImages.X, spriteSettings.SubImages.Y);

		GLsizei partileLen = (GLsizei)(particleCmp->GetEmitter()->GetInstancesCount());
		const TextureResource* Texture = particleCmp->GetEmitter()->GetSpritesheet();
		const GLParticleDeviceProxy* particleProxy = static_cast<const GLParticleDeviceProxy*>(particleCmp->GetEmitter()->GetParticleProxy());
		GLuint particleVAO = particleProxy->GetVAO();

		GLuint TextureID = Texture == nullptr
			? RDI->FallbackWhiteTexture
			: static_cast<const GLTextureDeviceProxy*>(Texture->GetTextureProxy())->GetTextureID();

		ParticleShader.SetUniform("uHasSprite", Texture == nullptr ? 0.0f : 1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		glBindVertexArray(particleVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, partileLen);
		glBindVertexArray(0);
		CHECK_GL_ERR();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::LinearizeDepth(const SceneView& sceneView)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Weirdly, moving this call drops performance into the floor

	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, linearDepth, 0);

	LinearizeDepthShader.BindProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, preDepthBuffer);
	LinearizeDepthShader.SetUniform("uDepth", 0);
	LinearizeDepthShader.SetUniform("uNear", sceneView.cameraCmp->GetClippingPlaneNear());
	LinearizeDepthShader.SetUniform("uFar", sceneView.cameraCmp->GetClippingPlaneFar());

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postColorBuffer1, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void TiledForwardRenderer::PostTonemapper(const SceneView& sceneView)
{
	// gConsole.LogInfo("TiledForwardRenderer::Tonemapper");

	float exposure = 1.0f;
	const PostprocessSettingsComponent* postCmp = sceneView.cameraCmp->GetSibling<PostprocessSettingsComponent>();
	if (postCmp) {
		exposure = postCmp->Exposure;
	}

	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Weirdly, moving this call drops performance into the floor
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost0);	

	hdrShader.BindProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	hdrShader.SetUniform("uExposure", exposure);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostSSAO(const SceneView& sceneView)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost1);

	// based on: http://john-chapman-graphics.blogspot.com/2013/01/ssao-tutorial.html
	static Dynarray<Vector> SSAOKernel;
	if (SSAOKernel.GetSize() <= 0) {
		gConsole.LogInfo("TiledForwardRenderer::PostSSAO generating kernel");
		for (int i = 0; i < 16; ++i) {
			SSAOKernel.PushBack(Vector(
				RandomRange(-1.0f, 1.0f),
				RandomRange(-1.0f, 1.0f),
				RandomRange( 0.0f, 1.0f)
			));
			SSAOKernel[i].Normalize();
			SSAOKernel[i] *= RandomRange(0.0f, 1.0f);
			float scale = float(i) / 16.0;
			scale = Lerp(0.1f, 1.0f, scale * scale);
			SSAOKernel[i] *= scale;
			gConsole.LogInfo("TiledForwardRenderer::PostSSAO kernel #{} {}", i, SSAOKernel[i]);
		}
	}

	SSAOShader.BindProgram();
	for (int i = 0; i < 16; ++i) {
		SSAOShader.SetUniform(String("uKernel[") + String::From(i) + "]", SSAOKernel[i]);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postColorBuffer0);
	SSAOShader.SetUniform("uBeauty", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalBuffer);
	SSAOShader.SetUniform("uNormal", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, preDepthBuffer);
	SSAOShader.SetUniform("uDepth", 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, RDI->SSAONoiseMap);
	SSAOShader.SetUniform("uNoise", 3);
	SSAOShader.SetUniform("uScreenRes", Vector(
		RDI->GetScreenSize().Width,
		RDI->GetScreenSize().Height,
		1.0f / ((float)RDI->GetScreenSize().Width),
		1.0f / ((float)RDI->GetScreenSize().Height))
	);
	SSAOShader.SetUniform("uFarClippingPlane", sceneView.cameraCmp->GetClippingPlaneFar());
	SSAOShader.SetUniform("uBias", 0.3f);
	SSAOShader.SetUniform("uViewFromWorld", sceneView.cameraCmp->GetViewFromWorld());

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::UIText2D(const SceneView& sceneView)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBOpost0);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ScreenSize screen = RDI->GetScreenSize();
	Matrix ortho;
	ortho.SetOrthographic(
		sceneView.rect.GetMin().Y * screen.Height,
		sceneView.rect.GetMax().Y * screen.Height,
		sceneView.rect.GetMin().X * screen.Width,
		sceneView.rect.GetMax().X * screen.Width,
		-1,
		 1
	);
	text2DShader.BindProgram();
	text2DShader.SetUniform("u_projection", ortho);

	for (auto componentsTuple : sceneView.world->IterateComponents<ScreenSpaceTextComponent>())
	{
		ScreenSpaceTextComponent* textCmp = std::get<ScreenSpaceTextComponent*>(componentsTuple);
		Text2D& text = textCmp->GetText();
		text2DShader.SetUniform("u_textColor", text.GetFontColor());
		text2DShader.SetUniform("u_position", Vector((float)textCmp->GetScreenPosition().X, (float)textCmp->GetScreenPosition().Y, 0));
		text.UpdateDeviceBuffers();

		const GLTextFieldBufferDeviceProxy* textFieldBuffer = static_cast<const GLTextFieldBufferDeviceProxy*>(text.GetTextFieldBuffer());

		glBindVertexArray(textFieldBuffer->GetVAO());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, static_cast<const GLTextureDeviceProxy*>(text.GetFontTextureProxy())->GetTextureID());

		// Render glyph texture over quad
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(6 * textFieldBuffer->GetSize()));

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
	CHECK_GL_ERR();

	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TiledForwardRenderer::PostGamma()
{
	GammaShader.BindProgram();
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, postColorBuffer0);
	GammaShader.SetUniform("uGamma", 2.2f);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void TiledForwardRenderer::SetupLightsBufferFromScene()
{
	if (lightBuffer == 0)
	{
		return;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
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
	Dynarray<Vector> Positions;
	Dynarray<Vector> Color;
	Dynarray<Vector> RangeIntensity;

	int LightCounter = 0;
	for (const PointLightComponent* pointLightCmp : sceneView.PointLights)
	{
		const EntityTransform& transform = pointLightCmp->GetTransform();

		Positions.PushBack(transform.GetGlobalTranslation());
		Color.PushBack(Vector(pointLightCmp->GetColor()));
		RangeIntensity.PushBack(Vector(pointLightCmp->GetRange(), pointLightCmp->GetIntensity(), 0.0f));

		++LightCounter;

		if (LightCounter > MAX_NUM_LIGHTS)
		{
			gConsole.LogInfo("TiledForwardRenderer::UpdateLightsBufferFromScene more lights than supported by renderer({})", MAX_NUM_LIGHTS);
			break;
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	Light *lights = (Light*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < LightCounter; ++i)
	{
		Light &light = lights[i];
		light.Position = Positions[i];
		light.Color = Color[i];
		light.RangeIntensity = RangeIntensity[i];

		// 		gConsole.LogInfo("TiledForwardRenderer::UpdateLightsBufferFromScene set Position: {}, Radius: {}",
		// 			light.Position, light.Radius);
	}

	int remainingSlots = MAX_NUM_LIGHTS - LightCounter;
	for (int i = 0; i < remainingSlots; ++i)
	{
		Light &light = lights[LightCounter + i];
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
	debugQuadDepthPrepassShader.BindProgram();
	debugQuadDepthPrepassShader.SetUniform("uNear", sceneView.cameraCmp->GetClippingPlaneNear());
	debugQuadDepthPrepassShader.SetUniform("uFar", sceneView.cameraCmp->GetClippingPlaneFar());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, preDepthBuffer);

	glBindVertexArray(RDI->PrimitivesQuad->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void TiledForwardRenderer::DebugLightAccum(const SceneView& sceneView)
{
	float Time = (float)(sceneView.world->GetWorldComponent<TimeWorldComponent>()->GetGameplayTime());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	debugLightAccumShader.BindProgram();
	debugLightAccumShader.SetUniform("uTime", Time);
	debugLightAccumShader.SetUniform("uWorkGroupsX", (int)workGroupsX);
	debugLightAccumShader.SetUniform("uWorkGroupsY", (int)workGroupsY);
	debugLightAccumShader.SetUniform("uLightCount", (int)std::min(sceneView.PointLights.GetSize(), MAX_NUM_LIGHTS));

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, visibleLightIndicesBuffer);

	const Matrix& ClipFromWorld = sceneView.cameraCmp->GetClipFromWorld();
	for (auto componentsTuple : sceneView.world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const EntityTransform& transform = meshCmp->GetTransform();
		const Matrix& WorldFromModel = transform.GetWorldFromModel();
		debugLightAccumShader.SetUniform("uClipFromModel", ClipFromWorld * WorldFromModel);
		debugLightAccumShader.SetUniform("uWorldFromModel", WorldFromModel);

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