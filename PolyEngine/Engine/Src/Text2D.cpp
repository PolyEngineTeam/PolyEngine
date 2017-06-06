#include "EnginePCH.hpp"
#include "Text2D.hpp"

#include "OpenGLUtils.hpp"
#include "FontResource.hpp"
#include "ResourceManager.hpp"

using namespace Poly;

Text2D::~Text2D()
{
	if(VBO)
		glDeleteBuffers(1, &VBO);

	if(VAO)
		glDeleteVertexArrays(1, &VAO);

	if(Font)
		ResourceManager<FontResource>::Release(Font);
}

void Text2D::SetFont(const String& fontName)
{
	Font = ResourceManager<FontResource>::Load(fontName);
}

void Poly::Text2D::Draw() const
{
	UpdateDeviceBuffers();

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Font->GetFace(FontSize).TextureID);

	// Render glyph texture over quad
	glDrawArrays(GL_TRIANGLES, 0, 6 * Text.GetLength());

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	CHECK_GL_ERR();
}

void Text2D::UpdateDeviceBuffers() const
{
	if (!Dirty)
		return;
	Dirty = false;

	// Create buffers on GPU
	if (VAO == 0 || VBO == 0)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		CHECK_GL_ERR();
	}

	if(!Font)
		Font = ResourceManager<FontResource>::Load(FontName);

	const FontResource::FontFace& face = Font->GetFace(FontSize);

	// Iterate through all characters
	float x = 0;
	float y = 0;
	float scale = 1;
	Dynarray<float> vboData;
	for (size_t i = 0; i < Text.GetLength(); ++i)
	{
		char c = Text[i];
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

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vboData.GetSize(), vboData.GetData(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECK_GL_ERR();
}
