#include <PolyRenderingDeviceGLPCH.hpp>

#include <Pipeline/EnvCapture.hpp>
#include <GLRenderingDevice.hpp>
#include <Common/PrimitiveCube.hpp>

using namespace Poly;

EnvCapture::EnvCapture(GLRenderingDevice* rdi)
	: RDI(rdi),
	EquirectangularToCubemapShader("Shaders/equiToCubemap.vert.glsl", "Shaders/equiToCubemap.frag.glsl"),
	CubemapIrradianceShader("Shaders/cubemapIrradiance.vert.glsl", "Shaders/cubemapIrradiance.frag.glsl"),
	PrefilterCubemapShader("Shaders/prefilterCubemap.vert.glsl", "Shaders/prefilterCubemap.frag.glsl")
{
	EquirectangularToCubemapShader.RegisterUniform("sampler2D", "uEquirectangularMap");
	EquirectangularToCubemapShader.RegisterUniform("mat4", "uClipFromModel");

	CubemapIrradianceShader.RegisterUniform("sampler2D", "uEnvCubemap");
	CubemapIrradianceShader.RegisterUniform("mat4", "uClipFromModel");

	PrefilterCubemapShader.RegisterUniform("sampler2D", "uEnvironmentMap");
	PrefilterCubemapShader.RegisterUniform("mat4", "uClipFromModel");
	PrefilterCubemapShader.RegisterUniform("float", "uRoughness");
}

EnvCapture::~EnvCapture()
{
	if (HDRPanorama > 0)
		glDeleteTextures(1, &HDRPanorama);

	if (EnvCubemap > 0)
		glDeleteTextures(1, &EnvCubemap);

	if (IrradianceMap > 0)
		glDeleteTextures(1, &IrradianceMap);

	if (PrefilterMap > 0)
		glDeleteTextures(1, &PrefilterMap);
}

void EnvCapture::UpdateEnv(const SkyboxWorldComponent* skyboxCmp)
{
	if (skyboxCmp->GetPanorama() == nullptr)
	{
		gConsole.LogInfo("EnvCapture::UpdateEnv SkyboxWorldComponent panorama is null! Abording!");
		return;
	}

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	CaptureCubemap(skyboxCmp);

	CaptureDiffuseIrradiance();

	CaptureSpecularPrefilteredMap();

	IsDirty = false;
}

void EnvCapture::CaptureCubemap(const SkyboxWorldComponent* skyboxCmp)
{
	// create cube map resource for capture
	gConsole.LogInfo("EnvCapture::CaptureCubemap create cubemap resource");
	glGenTextures(1, &EnvCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubemap);
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
	gConsole.LogInfo("EnvCapture::CaptureCubemap cubemap resource created");
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERR();

	// Create FBO for HDR capture
	gConsole.LogInfo("EnvCapture::CaptureCubemap Create capture FBO");
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	gConsole.LogInfo("EnvCapture::CaptureCubemap Capture FBO created");
	CHECK_GL_ERR();

	// Projection
	gConsole.LogInfo("EnvCapture::CaptureCubemap create matrices");
	Matrix uClipFromView;
	uClipFromView.SetPerspective(90.0_deg, 1.0f, 0.1f, 10.0f);

	Matrix ViewFromModel[] =
	{
		Matrix(Vector::ZERO, Vector(1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector::ZERO, Vector(-1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector::ZERO, Vector(0.0f, -1.0f,  0.0f), Vector(0.0f,  0.0f, -1.0f)),
		Matrix(Vector::ZERO, Vector(0.0f,  1.0f,  0.0f), Vector(0.0f,  0.0f,  1.0f)),
		Matrix(Vector::ZERO, Vector(0.0f,  0.0f,  1.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector::ZERO, Vector(0.0f,  0.0f, -1.0f), Vector(0.0f, -1.0f,  0.0f))
	};
	gConsole.LogInfo("EnvCapture::CaptureCubemap matrices created");

	// convert HDR equirectangular environment map to cubemap equivalent
	gConsole.LogInfo("EnvCapture::CaptureCubemap start cubemap rendering loop");
	EquirectangularToCubemapShader.BindProgram();
	EquirectangularToCubemapShader.SetUniform("uEquirectangularMap", 0);
	glActiveTexture(GL_TEXTURE0);
	HDRPanorama = (GLuint)(skyboxCmp->GetPanorama()->GetTextureProxy()->GetResourceID());
	glBindTexture(GL_TEXTURE_2D, HDRPanorama);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		gConsole.LogInfo("EnvCapture::CaptureCubemap cubemap rendering: {}", i);
		EquirectangularToCubemapShader.SetUniform("uClipFromModel", uClipFromView * ViewFromModel[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, EnvCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// renderCube(); // renders a 1x1 cube
		glBindVertexArray(RDI->PrimitivesCube->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	gConsole.LogInfo("EnvCapture::CaptureCubemap cubemap rendered");

	gConsole.LogInfo("EnvCapture::CaptureCubemap generate env cubemap mipmaps");
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	gConsole.LogInfo("EnvCapture::CaptureCubemap env cubemap mipmaps generated");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	CHECK_GL_ERR();
}

void EnvCapture::CaptureDiffuseIrradiance()
{
	glGenTextures(1, &IrradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Create FBO for HDR capture
	gConsole.LogInfo("EnvCapture::CaptureIrradiance Create capture FBO");
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	gConsole.LogInfo("EnvCapture::CaptureIrradiance Capture FBO created");

	// Projection
	gConsole.LogInfo("EnvCapture::CaptureIrradiance create matrices");
	Matrix uClipFromView;
	uClipFromView.SetPerspective(90.0_deg, 1.0f, 0.1f, 10.0f);

	Matrix ViewFromModel[] =
	{
		Matrix(Vector::ZERO, Vector(1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector::ZERO, Vector(-1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector::ZERO, Vector(0.0f, -1.0f,  0.0f), Vector(0.0f,  0.0f, -1.0f)),
		Matrix(Vector::ZERO, Vector(0.0f,  1.0f,  0.0f), Vector(0.0f,  0.0f,  1.0f)),
		Matrix(Vector::ZERO, Vector(0.0f,  0.0f,  1.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector::ZERO, Vector(0.0f,  0.0f, -1.0f), Vector(0.0f, -1.0f,  0.0f))
	};
	gConsole.LogInfo("EnvCapture::CaptureIrradiance matrices created");

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	CubemapIrradianceShader.BindProgram();
	CubemapIrradianceShader.SetUniform("uEnvCubemap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		CubemapIrradianceShader.SetUniform("uClipFromModel", uClipFromView * ViewFromModel[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IrradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(RDI->PrimitivesCube->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	CHECK_GL_ERR();
}

void EnvCapture::CaptureSpecularPrefilteredMap()
{
	gConsole.LogInfo("EnvCapture::CaptureSpecularPrefilteredMap create cubemap resource");

	int maxCaptureSize = 512;

	glGenTextures(1, &PrefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, PrefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, maxCaptureSize, maxCaptureSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	gConsole.LogInfo("EnvCapture::CaptureSpecularPrefilteredMap cubemap resource created");

	// Create FBO for HDR capture
	gConsole.LogInfo("EnvCapture::CaptureSpecularPrefilteredMap Create capture FBO");
	GLuint captureFBO;
	GLuint captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, maxCaptureSize, maxCaptureSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	gConsole.LogInfo("EnvCapture::CaptureSpecularPrefilteredMap Capture FBO created");

	// Projection
	gConsole.LogInfo("EnvCapture::CaptureSpecularPrefilteredMap create matrices");
	Matrix uClipFromView;
	uClipFromView.SetPerspective(90.0_deg, 1.0f, 0.1f, 10.0f);

	Matrix ViewFromModel[] =
	{
		Matrix(Vector::ZERO, Vector(1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector::ZERO, Vector(-1.0f,  0.0f,  0.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector::ZERO, Vector(0.0f, -1.0f,  0.0f), Vector(0.0f,  0.0f, -1.0f)),
		Matrix(Vector::ZERO, Vector(0.0f,  1.0f,  0.0f), Vector(0.0f,  0.0f,  1.0f)),
		Matrix(Vector::ZERO, Vector(0.0f,  0.0f,  1.0f), Vector(0.0f, -1.0f,  0.0f)),
		Matrix(Vector::ZERO, Vector(0.0f,  0.0f, -1.0f), Vector(0.0f, -1.0f,  0.0f))
	};

	gConsole.LogInfo("EnvCapture::CaptureSpecularPrefilteredMap capture prefiltered cubemap");
	PrefilterCubemapShader.BindProgram();
	PrefilterCubemapShader.SetUniform("uEnvironmentMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = (unsigned int)((float)maxCaptureSize * std::pow(0.5f, mip)); // why not bitshift?
		unsigned int mipHeight = (unsigned int)((float)maxCaptureSize * std::pow(0.5f, mip));
		gConsole.LogInfo("EnvCapture::CaptureSpecularPrefilteredMap capture prefiltered cubemap mip: {}, width: {}, height: {}", mip, mipWidth, mipHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		PrefilterCubemapShader.SetUniform("uRoughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			PrefilterCubemapShader.SetUniform("uClipFromModel", uClipFromView * ViewFromModel[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, PrefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(RDI->PrimitivesCube->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	gConsole.LogInfo("EnvCapture::CaptureSpecularPrefilteredMap prefiltered cubemap captured");

	CHECK_GL_ERR();
}