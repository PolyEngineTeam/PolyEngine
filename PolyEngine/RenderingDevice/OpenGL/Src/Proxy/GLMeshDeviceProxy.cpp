#include <PolyRenderingDeviceGLPCH.hpp>

#include <Proxy/GLMeshDeviceProxy.hpp>
#include <Common/GLUtils.hpp>

using namespace Poly;

//---------------------------------------------------------------
GLMeshDeviceProxy::GLMeshDeviceProxy()
{
	VBO[eBufferType::VERTEX_BUFFER] = 0;
	VBO[eBufferType::TEXCOORD_BUFFER] = 0;
	VBO[eBufferType::NORMAL_BUFFER] = 0;
	VBO[eBufferType::TANGENT_BUFFER] = 0;
	VBO[eBufferType::INDEX_BUFFER] = 0;
	VBO[eBufferType::BONE_INDEX_BUFFER] = 0;
	VBO[eBufferType::BONE_WEIGHT_BUFFER] = 0;
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

	if(VBO[eBufferType::TANGENT_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::TANGENT_BUFFER]);

	if (VBO[eBufferType::INDEX_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::INDEX_BUFFER]);

	if (VBO[eBufferType::BONE_INDEX_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::BONE_INDEX_BUFFER]);

	if (VBO[eBufferType::BONE_WEIGHT_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::BONE_WEIGHT_BUFFER]);

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

	if (mesh.HasVertices())
	{
		EnsureVBOCreated(eBufferType::VERTEX_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetPositions().size() * sizeof(core::math::Vector3f), mesh.GetPositions().data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
		CHECK_GL_ERR();
	}

	if (mesh.HasTextCoords())
	{
		EnsureVBOCreated(eBufferType::TEXCOORD_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::TEXCOORD_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetTextCoords().size() * sizeof(Mesh::TextCoord), mesh.GetTextCoords().data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
		CHECK_GL_ERR();
	}

	if (mesh.HasNormals())
	{
		EnsureVBOCreated(eBufferType::NORMAL_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetNormals().size() * sizeof(core::math::Vector3f), mesh.GetNormals().data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);
		CHECK_GL_ERR();
	}

	if(mesh.HasTangents())
	{
		EnsureVBOCreated(eBufferType::TANGENT_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::TANGENT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetTangents().size() * sizeof(core::math::Vector3f), mesh.GetTangents().data(), GL_STATIC_DRAW);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(3);
		CHECK_GL_ERR();
	}

	if(mesh.HasBitangents())
	{
		EnsureVBOCreated(eBufferType::BITANGENT_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::BITANGENT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetBitangents().size() * sizeof(core::math::Vector3f), mesh.GetBitangents().data(), GL_STATIC_DRAW);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(4);
		CHECK_GL_ERR();
	}

	if (mesh.HasBoneWeights())
	{
		EnsureVBOCreated(eBufferType::BONE_WEIGHT_BUFFER);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::BONE_WEIGHT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetBoneWeights().size() * sizeof(float) * 4, mesh.GetBoneWeights().data(), GL_STATIC_DRAW);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(5);

		// float* w = (float*)mesh.GetBoneWeights().data();
		// for (size_t i = 0; i < mesh.GetBoneWeights().size() * 4; i+=4)
		// {
		// 	core::utils::gConsole.LogError("W= ({}) {} {} {} {}", i/4, w[i], w[i + 1], w[i + 2], w[i + 3]);
		// }
	
		CHECK_GL_ERR();
	}

	if (mesh.HasBoneIds())
	{
		EnsureVBOCreated(eBufferType::BONE_INDEX_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::BONE_INDEX_BUFFER]);
		// glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetBoneIds().size() * sizeof(core::math::VectorT<u8, 4>), mesh.GetBoneIds().data(), GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, mesh.GetBoneIds().size() * sizeof(i8) * 4, mesh.GetBoneIds().data(), GL_STATIC_DRAW);
		glVertexAttribIPointer(6, 4, GL_BYTE, 0, NULL);
		glEnableVertexAttribArray(6);
		CHECK_GL_ERR();
	}

	if (mesh.HasIndicies())
	{
		EnsureVBOCreated(eBufferType::INDEX_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[eBufferType::INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndicies().size() * sizeof(GLuint), mesh.GetIndicies().data(), GL_STATIC_DRAW);

		glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(7);
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
