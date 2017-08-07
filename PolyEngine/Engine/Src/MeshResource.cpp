#include "EnginePCH.hpp"

#include "MeshResource.hpp"

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

Poly::MeshResource::~MeshResource()
{
	for (SubMesh* subMesh : SubMeshes)
	{
		delete subMesh;
	}
}

Poly::MeshResource::SubMesh::SubMesh(const String& path, aiMesh* mesh, aiMaterial* material)
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

		MeshData.DiffuseTexture = ResourceManager<TextureResource>::Load(textPath, eResourceSource::NONE);
		if (!MeshData.DiffuseTexture) {
			gConsole.LogError("Failed to load diffuse texture: {}", fullPath);
		} else {
			gConsole.LogDebug("Succeded to load diffuse texture: {}", fullPath);
		}
	} else {
		gConsole.LogError("Failed to load diffuse texture for material: {}", path);
		MeshData.DiffuseTexture = nullptr;
	}

	// Material params loading
	if (material->Get(AI_MATKEY_SHININESS_STRENGTH, MeshData.Mtl.SpecularIntensity) != AI_SUCCESS) {
		MeshData.Mtl.SpecularIntensity = 0.0f;
		gConsole.LogError("Error reading specular intensity in {}", path);
	}
	if (material->Get(AI_MATKEY_SHININESS, MeshData.Mtl.SpecularPower) != AI_SUCCESS) {
		MeshData.Mtl.SpecularPower = 0.0f;
		gConsole.LogError("Error reading specular power in {}", path);
	}
	aiColor3D color;
	material->Get(AI_MATKEY_COLOR_SPECULAR, color);
	MeshData.Mtl.SpecularColor = Color(color.r, color.b, color.g);

	gConsole.LogDebug("Specular params: {}, {}, [{},{},{}]", MeshData.Mtl.SpecularIntensity,
		MeshData.Mtl.SpecularPower, MeshData.Mtl.SpecularColor.R, MeshData.Mtl.SpecularColor.G,
		MeshData.Mtl.SpecularColor.B);


}
