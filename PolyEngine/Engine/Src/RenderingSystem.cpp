#include "EnginePCH.hpp"

#include "ShaderProgram.hpp"

#include <GL/glew.h>

using namespace Poly;

void RenderingSystem::RenderingPhase(World* world)
{	
	IRenderingContext* context = world->GetEngine()->GetRenderingContext();
	
	glDepthMask(GL_TRUE);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	ScreenSize screen = world->GetEngine()->GetRenderingContext()->GetScreenSize();
	for (auto& kv : world->GetViewportWorldComponent().GetViewports())
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		const AARect& rect = kv.second.GetRect();
		glViewport((int)(rect.GetMin().X * screen.Width), (int)(rect.GetMin().Y * screen.Height),
			(int)(rect.GetSize().X * screen.Width), (int)(rect.GetSize().Y * screen.Height));

		context->GetProgram(eShaderProgramType::TEST).BindProgram();
		const Matrix& mvp = kv.second.GetCamera()->GetMVP();
		//testProgram.SetUniform("uMVP", mvp);
		
		// Render objects
		for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
		{
			const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
			const TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

			const Matrix& objTransform = transCmp->GetGlobalTransformationMatrix();
			Matrix screenTransform = mvp * objTransform;
			context->GetProgram(eShaderProgramType::TEST).SetUniform("uTransform", screenTransform);
			for (const GLMeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
			{
				glBindVertexArray(subMesh->GetVAO());
				
				if (subMesh->GetDiffTexture())
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, subMesh->GetDiffTexture()->GetID());
				}
				
				glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetVertexCount(), GL_UNSIGNED_INT, NULL);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindVertexArray(0);
			}
		}
		if (gCoreConfig.DebugNormalsFlag) 
		{
			const Matrix& mModelView = kv.second.GetCamera()->GetModelViewMatrix();
			const Matrix& mProjection = kv.second.GetCamera()->GetProjectionMatrix();

			context->GetProgram(eShaderProgramType::DEBUG_NORMALS).BindProgram();
			context->GetProgram(eShaderProgramType::DEBUG_NORMALS).SetUniform("u_projection", mProjection);

			for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())	
			{
				const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
				const TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

				const Matrix& objTransform = transCmp->GetGlobalTransformationMatrix();
				Matrix MVPTransform = mvp * objTransform;
				Matrix mNormalMatrix = (mModelView * objTransform).GetInversed().GetTransposed();
				context->GetProgram(eShaderProgramType::DEBUG_NORMALS).SetUniform("u_MVP", MVPTransform);
				context->GetProgram(eShaderProgramType::DEBUG_NORMALS).SetUniform("u_normalMatrix4x4", mNormalMatrix);
				for (const GLMeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
				{
					glBindVertexArray(subMesh->GetVAO());
					glDrawElements(GL_TRIANGLES, subMesh->GetVertexCount(), GL_UNSIGNED_INT, NULL);
					glBindVertexArray(0);
				}
			}
		}

		CHECK_GL_ERR();
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}

	context->EndFrame();
}
