#include "EnginePCH.hpp"

#include "GLMeshResource.hpp"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <GL/glew.h>

using namespace Poly;

GLMeshResource::GLMeshResource(const String& path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path.GetCStr(), aiProcessPreset_TargetRealtime_Fast);

	if (!scene) {
		gConsole.LogError("Error Importing Asset: {}", importer.GetErrorString());
		throw ResourceLoadFailedException();
	}

	gConsole.LogDebug("Loading model {} sucessfull.", path);
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		SubMeshes.PushBack(new SubMesh(path, scene->mMeshes[i], scene->mMaterials[i]));
	}
}

Poly::GLMeshResource::~GLMeshResource()
{
	for (SubMesh* subMesh : SubMeshes)
	{
		delete subMesh;
	}
}

Poly::GLMeshResource::SubMesh::SubMesh(const String& path, aiMesh* mesh, aiMaterial* material)
{
	VBO[eBufferType::VERTEX_BUFFER] = 0;
	VBO[eBufferType::TEXCOORD_BUFFER] = 0;
	VBO[eBufferType::NORMAL_BUFFER] = 0;
	VBO[eBufferType::INDEX_BUFFER] = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	VertexCount = mesh->mNumFaces * 3;

	if (mesh->HasPositions()) {
		Dynarray<float> vertices;
		vertices.Resize(mesh->mNumVertices * 3);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			vertices[i * 3] = mesh->mVertices[i].x;
			vertices[i * 3 + 1] = mesh->mVertices[i].y;
			vertices[i * 3 + 2] = mesh->mVertices[i].z;
		}

		glGenBuffers(1, &VBO[eBufferType::VERTEX_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::VERTEX_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat),
			&vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
	}
	CHECK_GL_ERR();

	if (mesh->HasTextureCoords(0)) {
		Dynarray<float> texCoords;
		texCoords.Resize(mesh->mNumVertices * 2);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}

		glGenBuffers(1, &VBO[eBufferType::TEXCOORD_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::TEXCOORD_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat),
			&texCoords[0], GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
	}
	CHECK_GL_ERR();

	if (mesh->HasNormals()) {
		Dynarray<float> normals;
		normals.Resize(mesh->mNumVertices * 3);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			normals[i * 3] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;
		}

		glGenBuffers(1, &VBO[eBufferType::NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[eBufferType::NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat),
			&normals[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);
	}
	CHECK_GL_ERR();

	if (mesh->HasFaces()) {
		Dynarray<unsigned int> indices;
		indices.Resize(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			indices[i * 3] = mesh->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}

		glGenBuffers(1, &VBO[eBufferType::INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[eBufferType::INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint),
			&indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(3);
	}
	CHECK_GL_ERR();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	gConsole.LogDebug(
		"Loaded mesh entry: {} with {} vertices, {} faces and parameters: "
		"pos[{}], tex_coord[{}], norm[{}], faces[{}]",
		mesh->mName.C_Str(), VertexCount, mesh->mNumFaces,
		mesh->HasPositions() ? "on" : "off",
		mesh->HasTextureCoords(0) ? "on" : "off",
		mesh->HasNormals() ? "on" : "off", mesh->HasFaces() ? "on" : "off");

	// Material loading
	aiString texPath;
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
	{
		//TODO load textures, this requires Path class
		// temporary code for extracting path
		std::string tmpPath = std::string(path.GetCStr());
		std::replace(tmpPath.begin(), tmpPath.end(), '\\', '/'); // replace all '\' to '/', fix for linux machines
		std::string fullPath = tmpPath.substr(0, tmpPath.rfind('/')+1) + std::string(texPath.C_Str());
		std::replace(fullPath.begin(), fullPath.end(), '\\', '/'); // replace all '\' to '/', fix for linux machines
		String textPath(fullPath.c_str());
		// end temporary code for extracting path

		DiffuseTexture = ResourceManager<GLTextureResource>::Load(textPath);
		if (!DiffuseTexture)
			gConsole.LogError("Failed to load diffuse texture: {}", fullPath);
		else
			gConsole.LogDebug("Succeded to load diffuse texture: {}", fullPath);
	}

	// Material params loading
	material->Get(AI_MATKEY_SHININESS_STRENGTH, Mtl.SpecularIntensity);
	material->Get(AI_MATKEY_SHININESS, Mtl.SpecularPower);
	aiColor3D color;
	material->Get(AI_MATKEY_COLOR_SPECULAR, color);
	Mtl.SpecularColor = Color(color.r, color.b, color.g);

	gConsole.LogDebug("Specular params: {}, {}, [{},{},{}]", Mtl.SpecularIntensity,
		Mtl.SpecularPower, Mtl.SpecularColor.R, Mtl.SpecularColor.G,
		Mtl.SpecularColor.B);
}

Poly::GLMeshResource::SubMesh::~SubMesh()
{
	if (VBO[eBufferType::VERTEX_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::VERTEX_BUFFER]);

	if (VBO[eBufferType::TEXCOORD_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::TEXCOORD_BUFFER]);

	if (VBO[eBufferType::NORMAL_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::NORMAL_BUFFER]);

	if (VBO[eBufferType::INDEX_BUFFER])
		glDeleteBuffers(1, &VBO[eBufferType::INDEX_BUFFER]);

	if (DiffuseTexture)
		ResourceManager<GLTextureResource>::Release(DiffuseTexture);
	
	glDeleteVertexArrays(1, &VAO);
}
