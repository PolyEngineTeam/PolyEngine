#include "EnginePCH.hpp"

#include "Resources/MeshResource.hpp"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

using namespace Poly;

MeshResource::MeshResource(const String& path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path.GetCStr(), aiProcessPreset_TargetRealtime_Fast);

	if (!scene) {
		gConsole.LogError("Error Importing Asset: {}", importer.GetErrorString());
		throw ResourceLoadFailedException();
	}

	gConsole.LogDebug("Loading model {} sucessfull.", path);
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		SubMeshes.PushBack(new SubMesh(path, scene->mMeshes[i], scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]));
	}
}

MeshResource::~MeshResource()
{
	for (SubMesh* subMesh : SubMeshes)
	{
		delete subMesh;
	}
}

MeshResource::SubMesh::SubMesh(const String& path, aiMesh* mesh, aiMaterial* material)
{
	LoadGeometry(mesh);
	
	MeshData.EmissiveMap = LoadTexture(material, path, aiTextureType_EMISSIVE, eTextureUsageType::EMISSIVE);
	MeshData.AlbedoMap = LoadTexture(material, path, aiTextureType_DIFFUSE, eTextureUsageType::ALBEDO);
	MeshData.MetallicMap = LoadTexture(material, path, aiTextureType_SPECULAR, eTextureUsageType::METALLIC);
	MeshData.RoughnessMap = LoadTexture(material, path, aiTextureType_SHININESS, eTextureUsageType::ROUGHNESS);
	MeshData.NormalMap = LoadTexture(material, path, aiTextureType_HEIGHT, eTextureUsageType::NORMAL);
	MeshData.AmbientOcclusionMap = LoadTexture(material, path, aiTextureType_AMBIENT, eTextureUsageType::AMBIENT_OCCLUSION);
}

void MeshResource::SubMesh::LoadGeometry(aiMesh* mesh)
{
	if (mesh->HasPositions()) {
		MeshData.Positions.Resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Positions[i].X = mesh->mVertices[i].x;
			MeshData.Positions[i].Y = mesh->mVertices[i].y;
			MeshData.Positions[i].Z = mesh->mVertices[i].z;
		}
	}

	if (mesh->HasTextureCoords(0)) {
		MeshData.TextCoords.Resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.TextCoords[i].U = mesh->mTextureCoords[0][i].x;
			MeshData.TextCoords[i].V = mesh->mTextureCoords[0][i].y;
		}
	}

	if (mesh->HasNormals()) {
		MeshData.Normals.Resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Normals[i].X = mesh->mNormals[i].x;
			MeshData.Normals[i].Y = mesh->mNormals[i].y;
			MeshData.Normals[i].Z = mesh->mNormals[i].z;
		}
	}

	if (mesh->HasTangentsAndBitangents()) {
		MeshData.Tangents.Resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Tangents[i].X = mesh->mTangents[i].x;
			MeshData.Tangents[i].Y = mesh->mTangents[i].y;
			MeshData.Tangents[i].Z = mesh->mTangents[i].z;
		}

		MeshData.Bitangents.Resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Bitangents[i].X = mesh->mBitangents[i].x;
			MeshData.Bitangents[i].Y = mesh->mBitangents[i].y;
			MeshData.Bitangents[i].Z = mesh->mBitangents[i].z;
		}
	}

	if (mesh->HasFaces()) {
		MeshData.Indices.Resize(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			MeshData.Indices[i * 3] = mesh->mFaces[i].mIndices[0];
			MeshData.Indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			MeshData.Indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}
	}

	MeshProxy = gEngine->GetRenderingDevice()->CreateMesh();
	MeshProxy->SetContent(MeshData);

	gConsole.LogDebug(
		"Loaded mesh entry: {} with {} vertices, {} faces and parameters: "
		"pos[{}], tex_coord[{}], norm[{}], faces[{}]",
		mesh->mName.C_Str(), MeshData.Positions.GetSize(), mesh->mNumFaces,
		mesh->HasPositions() ? "on" : "off",
		mesh->HasTextureCoords(0) ? "on" : "off",
		mesh->HasNormals() ? "on" : "off", mesh->HasFaces() ? "on" : "off");
}

TextureResource* MeshResource::SubMesh::LoadTexture(aiMaterial* material, const String& path, aiTextureType aiType, eTextureUsageType textureType)
{
	TextureResource* texture = nullptr;

	aiString texturePath;
	if (material->GetTexture(aiType, 0, &texturePath) == AI_SUCCESS)
	{
		//TODO load textures, this requires Path class
		// temporary code for extracting path
		std::string tmpPath = std::string(path.GetCStr());
		std::replace(tmpPath.begin(), tmpPath.end(), '\\', '/'); // replace all '\' to '/', fix for linux machines
		std::string fullPath = tmpPath.substr(0, tmpPath.rfind('/') + 1) + std::string(texturePath.C_Str());
		std::replace(fullPath.begin(), fullPath.end(), '\\', '/'); // replace all '\' to '/', fix for linux machines
		String textPath(fullPath.c_str());
		// end temporary code for extracting path

		texture = ResourceManager<TextureResource>::Load(textPath, eResourceSource::NONE, textureType);
		if (!texture) {
			gConsole.LogError("Failed to load texture: {}", textPath);
		}
		else {
			gConsole.LogDebug("Succeded to load texture: {}", textPath);
		}
	}
	else {
		gConsole.LogError("Failed to load texture for material: {}", path);
	}

	return texture;
}
