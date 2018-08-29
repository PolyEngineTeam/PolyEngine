#include "PolyRenderingDeviceGLPCH.hpp"

#include "Common/PrimitiveQuad.hpp"

using namespace Poly;

PrimitiveQuad::PrimitiveQuad()
{
	// quad with uv mapping
	static const float vertices[] = {
		// x,y,z, u,v
		-1.f,  1.f,  0.f,  0.f,  1.f,
		 1.f, -1.f,  0.f,  1.f,  0.f,
		 1.f,  1.f,  0.f,  1.f,  1.f,

		-1.f,  1.f,  0.f,  0.f,  1.f,
		-1.f, -1.f,  0.f,  0.f,  0.f,
		 1.f, -1.f,  0.f,  1.f,  0.f
	};

	glGenVertexArrays(1, &VAO);
	ASSERTE(VAO > 0, "PrimitiveQuad VAO creation failed!");
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	ASSERTE(VBO > 0, "PrimitiveQuad VBO creation failed!");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

PrimitiveQuad::~PrimitiveQuad()
{
	if (VBO)
		glDeleteBuffers(1, &VBO);
	if (VAO)
		glDeleteVertexArrays(1, &VAO);
}