#include "PostprocessRenderingPass.hpp"

#include "GLRenderingDevice.hpp"
#include "GLMeshDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"

#include <AABox.hpp>
#include <World.hpp>
#include <TimeSystem.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MovementSystem.hpp>
#include <MeshRenderingComponent.hpp>
#include <PostprocessSettingsComponent.hpp>

using namespace Poly;

struct PostprocessQuad : public BaseObject<>
{
public:
	PostprocessQuad()
	{
		// quad with uv mapping
		static const float vertices[] = {
			// x,y,z, u,v
			-1.f, 1.f, 0.f, 0.f, 1.f,
			1.f, -1.f, 0.f, 1.f, 0.f,
			1.f, 1.f, 0.f, 1.f, 1.f,

			-1.f, 1.f, 0.f, 0.f, 1.f,
			-1.f, -1.f, 0.f, 0.f, 0.f,
			1.f, -1.f, 0.f, 1.f, 0.f
		};

		glGenVertexArrays(1, &VAO);
		ASSERTE(VAO > 0, "PostprocessQuad VAO creation failed!");
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		ASSERTE(VBO > 0, "PostprocessQuad VBO creation failed!");
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	~PostprocessQuad()
	{
		if (VBO)
			glDeleteBuffers(1, &VBO);
		if (VAO)
			glDeleteVertexArrays(1, &VAO);
	}

	GLuint VAO = 0;
	GLuint VBO = 0;
};



RenderingPass::RenderingPass(const String& fragment)
	: RenderingPassBase("Shaders/postprocessCommonVert.shader", fragment)
{
}

RenderingPass::RenderingPass(const String& geometry, const String& fragment)
	: RenderingPassBase("Shaders/postprocessCommonVert.shader", geometry, fragment)
{
}


void RenderingPass::OnRun(World* world, const CameraComponent* camera, const AABox& rect)
{
	static const PostprocessQuad QUAD;

	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	float ResolutionX = rect.GetSize().X * gRenderingDevice->GetScreenSize().Width;
	float ResolutionY = rect.GetSize().X * gRenderingDevice->GetScreenSize().Height;
	const TransformComponent* CameraTransform = camera->GetSibling<TransformComponent>();
	Vector CameraPosition = CameraTransform->GetGlobalTranslation();
	Matrix CameraRotation = CameraTransform->GetGlobalRotation().ToRotationMatrix();

	GetProgram().BindProgram();

	GetProgram().SetUniform("uTime", Time);
	GetProgram().SetUniform("uResolution", ResolutionX, ResolutionY);

	GetProgram().SetUniform("uCameraPosition", CameraPosition);
	GetProgram().SetUniform("uCameraRotation", CameraRotation);

	// const PostprocessSettingsComponent* PostprocessSettings = camera->GetSibling<PostprocessSettingsComponent>();
	PostprocessSettingsComponent* PostprocessSettings = world->GetComponent<PostprocessSettingsComponent>(camera->GetOwnerID());
	if (PostprocessSettings == nullptr)
	{
		gConsole.LogInfo("void PostprocessRenderingPass::OnRun: PostprocessSettings is NULL");
	}
	else
	{
		GetProgram().SetUniform("uVignette",						PostprocessSettings->Vignette);
		GetProgram().SetUniform("uBloom",							PostprocessSettings->Bloom);
		GetProgram().SetUniform("uBloomThreshold",					PostprocessSettings->BloomThreshold);
		GetProgram().SetUniform("uDistortion",						PostprocessSettings->Distortion);
		GetProgram().SetUniform("uColorTempValue",					PostprocessSettings->ColorTempValue);
		GetProgram().SetUniform("uColorTempLuminancePreservation",	PostprocessSettings->ColorTempLuminancePreservation);
		GetProgram().SetUniform("uSaturation",						PostprocessSettings->Saturation);
		GetProgram().SetUniform("uGrain",							PostprocessSettings->Grain);
		GetProgram().SetUniform("uStripes",							PostprocessSettings->Stripes);
		GetProgram().SetUniform("uBrightness",						PostprocessSettings->Brightness);
		GetProgram().SetUniform("uContrast",						PostprocessSettings->Contrast);
		
		// gConsole.LogInfo("void PostprocessRenderingPass::OnRun: VignetteIntensity: {}", PostprocessSettings->VignetteIntensity);
	}

	glBindVertexArray(QUAD.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
