#include "DebugRenderingBuffers.hpp"

Poly::DebugRenderingBuffers::DebugRenderingBuffers()
{
	// create VAO and VBOs
	glGenVertexArrays(1, &VAO);
	ASSERTE(VAO > 0, "DebugRenderingBuffers VAO creation failed!");

	glGenBuffers(1, &VBO);
	ASSERTE(VBO > 0, "DebugRenderingBuffers VBO creation failed!");
}

Poly::DebugRenderingBuffers::~DebugRenderingBuffers()
{
	if (VBO)
		glDeleteBuffers(1, &VBO);
	if (VAO)
		glDeleteVertexArrays(1, &VAO);
}
