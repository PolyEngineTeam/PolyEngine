#include "GLTextFieldBufferDeviceProxy.hpp"
#include "GLUtils.hpp"

using namespace Poly;

//---------------------------------------------------------------
GLTextFieldBufferDeviceProxy::GLTextFieldBufferDeviceProxy()
{
	glGenVertexArrays(1, &VAO);
	if(VAO <= 0)
		throw RenderingDeviceProxyCreationFailedException();

	glGenBuffers(1, &VBO);
	if (VBO <= 0)
		throw RenderingDeviceProxyCreationFailedException();

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

//---------------------------------------------------------------
GLTextFieldBufferDeviceProxy::~GLTextFieldBufferDeviceProxy()
{
	if (VBO)
		glDeleteBuffers(1, &VBO);

	if (VAO)
		glDeleteVertexArrays(1, &VAO);
}

//---------------------------------------------------------------
void GLTextFieldBufferDeviceProxy::SetContent(size_t count, const TextFieldLetter* letters)
{
	ASSERTE(count > 0 && letters != nullptr, "Invalid arguments");

	Size = count;

	float x = 0;
	float y = 0;
	Dynarray<float> vboData(count * 36);
	for (size_t i = 0; i < count; ++i)
	{
		GLfloat xpos = letters[i].PosX;
		GLfloat ypos = letters[i].PosY;

		GLfloat w = letters[i].SizeX;
		GLfloat h = letters[i].SizeY;
		// Update VBO for each character

		float vertices[36] = {
			// tri1 (pos + uv) //0 min, 1 max
			xpos, ypos + h, 0.0f, 1.0f,		letters[i].MinU, letters[i].MinV,
			xpos, ypos, 0.0f, 1.0f,			letters[i].MinU, letters[i].MaxV,
			xpos + w, ypos, 0.0f, 1.0f,     letters[i].MaxU, letters[i].MaxV,

			// tri2
			xpos, ypos + h, 0.0f, 1.0f,		letters[i].MinU, letters[i].MinV,
			xpos + w, ypos, 0.0f, 1.0f,     letters[i].MaxU, letters[i].MaxV,
			xpos + w, ypos + h, 0.0f, 1.0f, letters[i].MaxU, letters[i].MinV
		};

		for (int k = 0; k < 36; ++k)
			vboData.PushBack(vertices[k]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vboData.GetSize(), vboData.GetData(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECK_GL_ERR();
}
