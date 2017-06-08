#include "GLRenderingDevice.hpp"

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#elif defined(__linux__)
#include <GL/glxew.h>
#endif

#include "OpenGLUtils.hpp"

#include <World.hpp>
#include <CoreConfig.hpp>

#include <MeshRenderingComponent.hpp>
#include <TransformComponent.hpp>
#include <ScreenSpaceTextComponent.hpp>
#include <ViewportWorldComponent.hpp>

#include <Viewport.hpp>
#include <CameraComponent.hpp>



void Poly::GLRenderingDevice::RenderWorld(World * world)
{
	glDepthMask(GL_TRUE);
	glClearColor(0.5, 0.8, 0.9, 1);  //Hack for game, revert to: glClearColor(0.2, 0.2, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if (gCoreConfig.WireframeRendering)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	ScreenSize screen = world->GetEngine()->GetRenderingContext()->GetScreenSize();
	for (auto& kv : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		const AABox& rect = kv.second.GetRect();
		glViewport((int)(rect.GetMin().X * screen.Width), (int)(rect.GetMin().Y * screen.Height),
			(int)(rect.GetSize().X * screen.Width), (int)(rect.GetSize().Y * screen.Height));

		GetProgram(eShaderProgramType::TEST).BindProgram();
		const Matrix& mvp = kv.second.GetCamera()->GetMVP();
		//testProgram.SetUniform("uMVP", mvp);
		// Render objects
		for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
		{
			const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
			const TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

			const Matrix& objTransform = transCmp->GetGlobalTransformationMatrix();
			Matrix screenTransform = mvp * objTransform;
			GetProgram(eShaderProgramType::TEST).SetUniform("uTransform", screenTransform);
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

			GetProgram(eShaderProgramType::DEBUG_NORMALS).BindProgram();
			GetProgram(eShaderProgramType::DEBUG_NORMALS).SetUniform("u_projection", mProjection);

			for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
			{
				const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
				const TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

				const Matrix& objTransform = transCmp->GetGlobalTransformationMatrix();
				Matrix MVPTransform = mvp * objTransform;
				Matrix mNormalMatrix = (mModelView * objTransform).GetInversed().GetTransposed();
				GetProgram(eShaderProgramType::DEBUG_NORMALS).SetUniform("u_MVP", MVPTransform);
				GetProgram(eShaderProgramType::DEBUG_NORMALS).SetUniform("u_normalMatrix4x4", mNormalMatrix);
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

		// Text drawing
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		CHECK_GL_ERR();

		Matrix ortho;
		ortho.SetOrthographic(rect.GetMin().Y * screen.Height, rect.GetMax().Y * screen.Height, rect.GetMin().X * screen.Width, rect.GetMax().X * screen.Width, -1, 1);
		GetProgram(eShaderProgramType::TEXT_2D).BindProgram();
		GetProgram(eShaderProgramType::TEXT_2D).SetUniform("u_projection", ortho);

		for (auto componentsTuple : world->IterateComponents<ScreenSpaceTextComponent>())
		{
			ScreenSpaceTextComponent* textCmp = std::get<ScreenSpaceTextComponent*>(componentsTuple);
			Text2D& text = textCmp->GetText();
			GetProgram(eShaderProgramType::TEXT_2D).SetUniform("u_textColor", text.GetFontColor());
			GetProgram(eShaderProgramType::TEXT_2D).SetUniform("u_position", textCmp->GetScreenPosition());
			text.Draw();
		}

		glDisable(GL_BLEND);
	}

	EndFrame();
}
