#include "TransparentRenderingPass.hpp"

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



TransparentRenderingPass::TransparentRenderingPass()
	: RenderingPassBase("Shaders/transparentVert.shader", "Shaders/transparentFrag.shader")
{
}


void TransparentRenderingPass::OnRun(World* world, const CameraComponent* camera, const AARect& rect)
{
	static const PostprocessQuad QUAD;

	float Time = (float)TimeSystem::GetTimerElapsedTime(world, eEngineTimer::GAMEPLAY);
	const TransformComponent* CameraTransform = camera->GetSibling<TransformComponent>();

	GetProgram().BindProgram();

	GetProgram().SetUniform("uTime", Time);
	// GetProgram().SetUniform("uResolution", ResolutionX, ResolutionY);
	//gConsole.LogInfo("void TransparentRenderingPass::OnRun: UseCashetes: {}", PostprocessSettings->UseCashetes);

	const float cameraHeight = 16.f + 1.f;
	float verticalSpan = cameraHeight / 2.0f;
	float horizontalSpan = (cameraHeight * camera->GetAspect()) / 2.0f;
	Vector cameraPos = camera->GetSibling<TransformComponent>()->GetGlobalTranslation();

	// Render meshes
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

		if (!meshCmp->IsTransparent())
			continue;

		Vector objPos = transCmp->GetGlobalTranslation();

		bool shouldCull = objPos.Y > cameraPos.Y + verticalSpan;
		shouldCull = shouldCull || objPos.Y < cameraPos.Y - verticalSpan;
		shouldCull = shouldCull || objPos.X > cameraPos.X + horizontalSpan;
		shouldCull = shouldCull || objPos.X < cameraPos.X - horizontalSpan;
		if (shouldCull)
			continue;

		GetProgram().SetUniform("uBaseColor", meshCmp->GetBaseColor());

		Matrix objScale;
		objScale.SetScale(Vector(1.0f, 1.0f, 1.0f));

		Matrix objTransform; // = transCmp->GetGlobalTransformationMatrix();
		objTransform.SetTranslation(transCmp->GetGlobalTranslation() + Vector(0.0f, 0.0f, 0.5f));
		
		Matrix screenTransform = camera->GetMVP() * objTransform * objScale;
		GetProgram().SetUniform("uTransform", objTransform * objScale);
		GetProgram().SetUniform("uMVPTransform", screenTransform);
		GetProgram().SetUniform("uBaseColor", meshCmp->GetBaseColor());
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());

			glBindVertexArray(QUAD.VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
}
