#include "GLMeshDeviceProxy.hpp"
#include "GLUtils.hpp"

using namespace Poly;

//---------------------------------------------------------------
GLMeshDeviceProxy::GLMeshDeviceProxy()
{
	VBO[eBufferType::VERTEX_BUFFER] = 0;
	VBO[eBufferType::TEXCOORD_BUFFER] = 0;
	VBO[eBufferType::NORMAL_BUFFER] = 0;
	VBO[eBufferType::INDEX_BUFFER] = 0;
}

//---------------------------------------------------------------
GLMeshDeviceProxy::~GLMeshDeviceProxy()
{
	if (VBO[eBufferType::VERTEX_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::VERTEX_BUFFER]);

	if (VBO[eBufferType::TEXCOORD_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::TEXCOORD_BUFFER]);

	if (VBO[eBufferType::NORMAL_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::NORMAL_BUFFER]);

	if (VBO[eBufferType::INDEX_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::INDEX_BUFFER]);

	if(VAO)
		glDeleteVertexArrays(1, &VAO);
}

//---------------------------------------------------------------
void GLMeshDeviceProxy::SetContent(const Mesh& mesh)
{
	if (!VAO)
	{
		glGenVertexArrays(1, &VAO);
		if (!VAO)
			throw RenderingDeviceProxyCreationFailedException();
	}
		
	glBindVertexArray(VAO);

	ASSERTE(mesh.HasVertices() && mesh.HasIndicies(), "Meshes that does not contain vertices and faces are not supported yet!");

	if (mesh.HasVertices()) {
		EnsureVBOCreated(eBufferType::VERTEX_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetPositions().GetSize() * sizeof(Mesh::Vector3D), mesh.GetPositions().GetData(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
		CHECK_GL_ERR();
	}

	if (mesh.HasTextCoords()) {
		EnsureVBOCreated(eBufferType::TEXCOORD_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::TEXCOORD_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetTextCoords().GetSize() * sizeof(Mesh::TextCoord), mesh.GetTextCoords().GetData(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
		CHECK_GL_ERR();
	}

	if (mesh.HasNormals()) {
		EnsureVBOCreated(eBufferType::NORMAL_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetNormals().GetSize() * sizeof(Mesh::Vector3D), mesh.GetNormals().GetData(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);
		CHECK_GL_ERR();
	}

	if (mesh.HasIndicies())
	{
		EnsureVBOCreated(eBufferType::INDEX_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[eBufferType::INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndicies().GetSize() * sizeof(GLuint), mesh.GetIndicies().GetData(), GL_STATIC_DRAW);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(3);
		CHECK_GL_ERR();
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//---------------------------------------------------------------
void GLMeshDeviceProxy::EnsureVBOCreated(eBufferType type)
{
	if (!VBO[type])
	{
		glGenBuffers(1, &VBO[type]);
		if (!VBO[type])
			throw RenderingDeviceProxyCreationFailedException();
	}
}
