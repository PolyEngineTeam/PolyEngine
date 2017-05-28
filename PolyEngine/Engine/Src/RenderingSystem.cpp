#include "EnginePCH.hpp"

#include "ShaderProgram.hpp"

#include <GL/glew.h>

using namespace Poly;

void DrawText2D(ShaderProgram& program, const FontResource::FontFace& face, const String& text, const Vector& pos, const Color& color)
{
	static bool once = true;
	
	static GLuint VAO, VBO;

	if (once)
	{
		once = false;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		CHECK_GL_ERR();
	}

	// Iterate through all characters
	const char* textData = text.GetCStr();
	float x = pos.X;
	float y = pos.Y;
	float scale = 1;
	Dynarray<float> vboData;
	for (size_t i = 0; i < text.GetLength(); ++i)
	{
		char c = text[i];
		auto it = face.Characters.find(c);
		FontResource::FontFace::FontGlyph ch = it->second;

		GLfloat xpos = x + ch.Bearing.X * scale;
		GLfloat ypos = y - (ch.Size.Y - ch.Bearing.Y) * scale;

		GLfloat w = ch.Size.X * scale;
		GLfloat h = ch.Size.Y * scale;
		// Update VBO for each character

		float vertices[36] = {
			// tri1 (pos + uv)
			xpos, ypos + h, 0.0f, 1.0f,		ch.TextureUV[0].X, ch.TextureUV[0].Y,
			xpos, ypos, 0.0f, 1.0f,			ch.TextureUV[0].X, ch.TextureUV[1].Y,
			xpos + w, ypos, 0.0f, 1.0f,     ch.TextureUV[1].X, ch.TextureUV[1].Y,

			// tri2
			xpos, ypos + h, 0.0f, 1.0f,		ch.TextureUV[0].X, ch.TextureUV[0].Y,
			xpos + w, ypos, 0.0f, 1.0f,     ch.TextureUV[1].X, ch.TextureUV[1].Y,
			xpos + w, ypos + h, 0.0f, 1.0f, ch.TextureUV[1].X, ch.TextureUV[0].Y
		};

		for (int k = 0; k < 36; ++k)
			vboData.PushBack(vertices[k]);

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += ch.Advance * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}

	program.SetUniform("u_textColor", color);
	
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, face.TextureID);
	CHECK_GL_ERR();

	// Render glyph texture over quad
	
	CHECK_GL_ERR();
	// Update content of VBO memory
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vboData.GetSize(), vboData.GetData(), GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECK_GL_ERR();
	// Render quad
	glDrawArrays(GL_TRIANGLES, 0, 6 * text.GetLength());

	
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	CHECK_GL_ERR();
}

void RenderingSystem::RenderingPhase(World* world)
{	
	IRenderingContext* context = world->GetEngine()->GetRenderingContext();
	
	glDepthMask(GL_TRUE);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if (gCoreConfig.WireframeRendering)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

		// tmp draw text
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		CHECK_GL_ERR();

		Matrix ortho;
		ortho.SetOrthographic(rect.GetMin().Y * screen.Height, rect.GetMax().Y * screen.Height, rect.GetMin().X * screen.Width, rect.GetMax().X * screen.Width, -1, 1);
		context->GetProgram(eShaderProgramType::TEXT_2D).BindProgram();
		context->GetProgram(eShaderProgramType::TEXT_2D).SetUniform("u_projection", ortho);

		FontResource* font = ResourceManager<FontResource>::Load("Fonts/Raleway/Raleway-Regular.ttf");
		DrawText2D(context->GetProgram(eShaderProgramType::TEXT_2D), font->GetFace(64), "test", Vector(0, screen.Height - 100, 0), Color(1,1,1));
		CHECK_GL_ERR();
		glDisable(GL_BLEND);
	}

	context->EndFrame();
}
