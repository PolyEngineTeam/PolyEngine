#include "GLRenderingDevice.hpp"

#include <World.hpp>
#include <CoreConfig.hpp>
#include <Viewport.hpp>

#include <CameraComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <TransformComponent.hpp>
#include <ScreenSpaceTextComponent.hpp>
#include <ViewportWorldComponent.hpp>

#include "GLTextFieldBufferDeviceProxy.hpp"
#include "GLTextureDeviceProxy.hpp"
#include "GLMeshDeviceProxy.hpp"
#include "GLUtils.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
void GLRenderingDevice::RenderWorld(World * world)
{
	// Prepare frame buffer
	glDepthMask(GL_TRUE);
	glClearColor(0.2, 0.2, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Choose correct rendering mode
	if (gCoreConfig.WireframeRendering)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	ScreenSize screen = gEngine->GetRenderingDevice()->GetScreenSize();

	// For each visible viewport draw it
	for (auto& kv : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		// Get viewport rect (TOOO change it to propper rect, not box)
		const AABox& rect = kv.second.GetRect();
		glViewport((int)(rect.GetMin().X * screen.Width), (int)(rect.GetMin().Y * screen.Height),
			(int)(rect.GetSize().X * screen.Width), (int)(rect.GetSize().Y * screen.Height));

		// Bind default drawing program (it's test for now)
		GetProgram(eShaderProgramType::TEST).BindProgram();

		// Get camera MVP matrix
		const Matrix& mvp = kv.second.GetCamera()->GetMVP();

		// Render meshes
		for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
		{
			const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
			const TransformComponent* transCmp = std::get<TransformComponent*>(componentsTuple);

			const Matrix& objTransform = transCmp->GetGlobalTransformationMatrix();
			Matrix screenTransform = mvp * objTransform;
			GetProgram(eShaderProgramType::TEST).SetUniform("uTransform", screenTransform);
			for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
			{
				const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());

				glBindVertexArray(meshProxy->VAO);

				if (subMesh->GetMeshData().GetDiffTexture())
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, static_cast<const GLTextureDeviceProxy*>(subMesh->GetMeshData().GetDiffTexture()->GetTextureProxy())->TextureID);
				}

				glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindVertexArray(0);
			}
		}
		CHECK_GL_ERR();

		// Draw debug normals
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
				for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
				{
					const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());
					glBindVertexArray(meshProxy->VAO);
					glDrawElements(GL_TRIANGLES, subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
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
			text.UpdateDeviceBuffers();

			const GLTextFieldBufferDeviceProxy* textFieldBuffer = static_cast<const GLTextFieldBufferDeviceProxy*>(text.GetTextFieldBuffer());

			glBindVertexArray(textFieldBuffer->VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, static_cast<const GLTextureDeviceProxy*>(text.GetFontTextureProxy())->TextureID);

			// Render glyph texture over quad
			glDrawArrays(GL_TRIANGLES, 0, 6 * textFieldBuffer->Size);

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		}
		CHECK_GL_ERR();

		glDisable(GL_BLEND);
	}

	// Signal frame end
	EndFrame();
}

