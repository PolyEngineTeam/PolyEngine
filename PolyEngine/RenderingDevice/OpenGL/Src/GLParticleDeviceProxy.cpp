#include "GLMeshDeviceProxy.hpp"
#include "Vector3f.hpp"
#include "GLUtils.hpp"
#include "GLParticleDeviceProxy.hpp"

using namespace Poly;

//---------------------------------------------------------------
GLParticleDeviceProxy::GLParticleDeviceProxy()
{
	// VBO[eBufferType::VERTEX_BUFFER] = 0;
	// VBO[eBufferType::TEXCOORD_BUFFER] = 0;
	// VBO[eBufferType::INDEX_BUFFER] = 0;
	//  VBO[eBufferType::NORMAL_BUFFER] = 0;
}

//---------------------------------------------------------------
GLParticleDeviceProxy::~GLParticleDeviceProxy()
{
	// if (VBO[eBufferType::VERTEX_BUFFER])
	// 	glDeleteBuffers(1, &VBO[eBufferType::VERTEX_BUFFER]);
	// 
	// if (VBO[eBufferType::TEXCOORD_BUFFER])
	// 	glDeleteBuffers(1, &VBO[eBufferType::TEXCOORD_BUFFER]);
	// 
	// if (VBO[eBufferType::NORMAL_BUFFER])
	// 	glDeleteBuffers(1, &VBO[eBufferType::NORMAL_BUFFER]);
	// 
	// if (VBO[eBufferType::INDEX_BUFFER])
	// 	glDeleteBuffers(1, &VBO[eBufferType::INDEX_BUFFER]);
	// 
	// if(VAO)
	// 	glDeleteVertexArrays(1, &VAO);
}

void Poly::GLParticleDeviceProxy::SetContent(const ParticleEmitter & particles)
{
	/*
	
	float quadVertices[] = {
		// positions			// uv
		-1.0f,  1.0f, 0.0f,		1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,		0.0f, 1.0f,
		1.0f,  1.0f, 0.0f,		0.0f, 0.0f,

		-1.0f,  1.0f, 0.0f,		1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 1.0f,
		1.0f, -1.0f, 0.0f,		0.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	instancesTransform.Resize(16 * instancesLen);

	// fill array with zeros
	for (int i = 0; i < 16 * instancesLen; ++i)
	{
		instancesTransform[i] = 0.0f;
	}

	srand(42);

	int index = 0;
	for (int i = 0; i < instancesLen; ++i)
	{
		// identity
		instancesTransform[index + 0] = 1.0f;
		instancesTransform[index + 5] = 1.0f;
		instancesTransform[index + 10] = 1.0f;
		instancesTransform[index + 15] = 1.0f;
		// translation
		instancesTransform[index + 12] = 5.0f * Random(-1.0, 1.0);
		instancesTransform[index + 13] = 5.0f * Random(-1.0, 1.0);
		instancesTransform[index + 14] = 5.0f * Random(-1.0, 1.0);
		index += 16;
	}

	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * instancesLen, instancesTransform.GetData(), GL_STATIC_DRAW);
	// glBindBuffer(GL_ARRAY_BUFFER, 0);

	// http://sol.gfxile.net/instancing.html
	// int pos = glGetAttribLocation((GLint)GetProgram().GetProgramHandle(), "aOffset");
	int pos = 3;
	int pos1 = pos + 0;
	int pos2 = pos + 1;
	int pos3 = pos + 2;
	int pos4 = pos + 3;
	glEnableVertexAttribArray(pos1);
	glEnableVertexAttribArray(pos2);
	glEnableVertexAttribArray(pos3);
	glEnableVertexAttribArray(pos4);
	// glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
	glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));
	glVertexAttribDivisor(pos1, 1);
	glVertexAttribDivisor(pos2, 1);
	glVertexAttribDivisor(pos3, 1);
	glVertexAttribDivisor(pos4, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	*/
}

//---------------------------------------------------------------
// void GLParticleDeviceProxy::SetContent(const Mesh& mesh)
// {
	// if (!VAO)
	// {
	// 	glGenVertexArrays(1, &VAO);
	// 	if (!VAO)
	// 		throw RenderingDeviceProxyCreationFailedException();
	// }
	// 	
	// glBindVertexArray(VAO);
	// 
	// ASSERTE(mesh.HasVertices() && mesh.HasIndicies(), "Meshes that does not contain vertices and faces are not supported yet!");
	// 
	// if (mesh.HasVertices()) {
	// 	EnsureVBOCreated(eBufferType::VERTEX_BUFFER);
	// 	glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::VERTEX_BUFFER]);
	// 	glBufferData(GL_ARRAY_BUFFER, mesh.GetPositions().GetSize() * sizeof(Vector3f), mesh.GetPositions().GetData(), GL_STATIC_DRAW);
	// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	// 	glEnableVertexAttribArray(0);
	// 	CHECK_GL_ERR();
	// }
	// 
	// if (mesh.HasTextCoords()) {
	// 	EnsureVBOCreated(eBufferType::TEXCOORD_BUFFER);
	// 	glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::TEXCOORD_BUFFER]);
	// 	glBufferData(GL_ARRAY_BUFFER, mesh.GetTextCoords().GetSize() * sizeof(Mesh::TextCoord), mesh.GetTextCoords().GetData(), GL_STATIC_DRAW);
	// 	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	// 	glEnableVertexAttribArray(1);
	// 	CHECK_GL_ERR();
	// }
	// 
	// if (mesh.HasNormals()) {
	// 	EnsureVBOCreated(eBufferType::NORMAL_BUFFER);
	// 	glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::NORMAL_BUFFER]);
	// 	glBufferData(GL_ARRAY_BUFFER, mesh.GetNormals().GetSize() * sizeof(Vector3f), mesh.GetNormals().GetData(), GL_STATIC_DRAW);
	// 	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	// 	glEnableVertexAttribArray(2);
	// 	CHECK_GL_ERR();
	// }
	// 
	// if (mesh.HasIndicies())
	// {
	// 	EnsureVBOCreated(eBufferType::INDEX_BUFFER);
	// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[eBufferType::INDEX_BUFFER]);
	// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndicies().GetSize() * sizeof(GLuint), mesh.GetIndicies().GetData(), GL_STATIC_DRAW);
	// 
	// 	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	// 	glEnableVertexAttribArray(3);
	// 	CHECK_GL_ERR();
	// }
	// 
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindVertexArray(0);
// }

//---------------------------------------------------------------
void GLParticleDeviceProxy::EnsureVBOCreated(eBufferType type)
{
	// if (!VBO[type])
	// {
	// 	glGenBuffers(1, &VBO[type]);
	// 	if (!VBO[type])
	// 		throw RenderingDeviceProxyCreationFailedException();
	// }
}
