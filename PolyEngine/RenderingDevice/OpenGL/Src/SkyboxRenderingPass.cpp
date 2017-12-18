#include "SkyboxRenderingPass.hpp"

#include "GLRenderingDevice.hpp"
// #include "GLMeshDeviceProxy.hpp"
// #include "GLTextureDeviceProxy.hpp"
#include "GLCubemapDeviceProxy.hpp"

#include <AABox.hpp>
#include <AARect.hpp>
#include <World.hpp>
#include <TimeSystem.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MovementSystem.hpp>
// #include <MeshRenderingComponent.hpp>
#include <SkyboxWorldComponent.hpp>

using namespace Poly;

SkyboxRenderingPass::SkyboxRenderingPass(const PostprocessQuad* quad)
	: RenderingPassBase("Shaders/skyboxVert.shader", "Shaders/skyboxFrag.shader"), Quad(quad)
{
	GetProgram().RegisterUniform("mat4", "uMVP");

	CreateCube();
}

void Poly::SkyboxRenderingPass::CreateCube()
{
	// cube
	static const float vertices[] = {
		// x, y, z 
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &CubeVAO);
	ASSERTE(CubeVAO > 0, "PostprocessQuad VAO creation failed!");
	glBindVertexArray(CubeVAO);
	glGenBuffers(1, &CubeVBO);
	ASSERTE(CubeVBO > 0, "PostprocessQuad VBO creation failed!");
	glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Poly::SkyboxRenderingPass::~SkyboxRenderingPass()
{
	if (CubeVBO)
		glDeleteBuffers(1, &CubeVBO);

	if (CubeVAO)
		glDeleteVertexArrays(1, &CubeVAO);
}

void SkyboxRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& rect, ePassType /*passType = ePassType::GLOBAL*/ )
{
	const SkyboxWorldComponent* SkyboxWorldCmp = world->GetWorldComponent<SkyboxWorldComponent>();
	if (SkyboxWorldCmp == nullptr)
	{
		gConsole.LogInfo("SkyboxRenderingPass::OnRun: SkyboxWorldCmp is NULL");
	}
	else
	{
		// float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	
		// const TransformComponent* CameraTransform = camera->GetSibling<TransformComponent>();
		// Vector CameraPosition = CameraTransform->GetGlobalTranslation();
		// Matrix CameraRotation = CameraTransform->GetGlobalRotation().ToRotationMatrix();
		// float ResolutionX = rect.GetSize().X * gRenderingDevice->GetScreenSize().Width;
		// float ResolutionY = rect.GetSize().Y * gRenderingDevice->GetScreenSize().Height;
	
		const Matrix projection = camera->GetProjectionMatrix();
		Matrix modelView = Matrix(camera->GetModelViewMatrix());
		// center cube in view space, SetTranslation resets Matrix to identity
		modelView.Data[3]  = 0.0f;
		modelView.Data[7]  = 0.0f;
		modelView.Data[11] = 0.0f;

		Matrix cubeTransform;
		cubeTransform.SetScale(1000.0f);

		Matrix mvp = projection * modelView * cubeTransform;

		GetProgram().BindProgram();
		GetProgram().SetUniform("uMVP", mvp);

		GLuint CubemapID = static_cast<const GLCubemapDeviceProxy*>(SkyboxWorldCmp->GetCubemap().GetTextureProxy())->GetTextureID();

		glDepthMask(GL_FALSE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);
		// glDepthFunc(GL_LESS);
		// glDepthFunc(GL_GREATER);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubemapID);

		// glBindVertexArray(Quad->VAO);
		glBindVertexArray(CubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		
		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);	
	}
}
