#include <EnginePCH.hpp>

#include <Resources/MeshResource.hpp>
#include <Resources/ResourceManager.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

RTTI_DEFINE_TYPE(Poly::MeshResource)

MeshResource::MeshResource(const String& path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path.GetCStr(), aiProcessPreset_TargetRealtime_Fast);

	if (!scene)
	{
		gConsole.LogError("Error Importing Asset: {}", importer.GetErrorString());
		throw ResourceLoadFailedException();
	}

	gConsole.LogDebug("Loading model {} sucessfull.", path);

	const float maxFloat = std::numeric_limits<float>::max();
	Vector min(maxFloat, maxFloat, maxFloat);
	Vector max(-maxFloat, -maxFloat, -maxFloat);
	
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		SubMeshes.PushBack(new SubMesh(path, scene->mMeshes[i], scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]));

		min = Vector::Min(min, SubMeshes[i]->GetAABox().GetMin());
		max = Vector::Max(max, SubMeshes[i]->GetAABox().GetMax());
	}

	for (size_t i = 0; i < scene->mNumAnimations; ++i)
	{
		Animations.PushBack(new Animation(scene->mAnimations[i]));
	}

	AxisAlignedBoundingBox = AABox(min, max - min);
}

MeshResource::~MeshResource()
{
	for (SubMesh* subMesh : SubMeshes)
	{
		delete subMesh;
	}
	for (Animation* animation : Animations)
	{
		delete animation;
	}
}

MeshResource::SubMesh::SubMesh(const String& path, aiMesh* mesh, aiMaterial* material)
{
	LoadGeometry(mesh);
	LoadBones(mesh);
	
	MeshData.EmissiveMap			= LoadTexture(material, path, (unsigned int)aiTextureType_EMISSIVE,		eTextureUsageType::EMISSIVE);
	MeshData.AlbedoMap				= LoadTexture(material, path, (unsigned int)aiTextureType_DIFFUSE,		eTextureUsageType::ALBEDO);
	MeshData.MetallicMap			= LoadTexture(material, path, (unsigned int)aiTextureType_SPECULAR,		eTextureUsageType::METALLIC);
	MeshData.RoughnessMap			= LoadTexture(material, path, (unsigned int)aiTextureType_SHININESS,	eTextureUsageType::ROUGHNESS);
	MeshData.NormalMap				= LoadTexture(material, path, (unsigned int)aiTextureType_HEIGHT,		eTextureUsageType::NORMAL);
	MeshData.AmbientOcclusionMap	= LoadTexture(material, path, (unsigned int)aiTextureType_AMBIENT,		eTextureUsageType::AMBIENT_OCCLUSION);
}

void MeshResource::SubMesh::LoadBones(aiMesh* mesh)
{
	if (mesh->HasBones())
	{
		for (size_t i = 0; i < mesh->mNumBones; ++i)
		{
			Bones.PushBack({ String(mesh->mBones[i]->mName.C_Str()) });
		}
	}
	else
	{
		gConsole.LogWarning("No bones");
	}
}

void MeshResource::SubMesh::LoadGeometry(aiMesh* mesh)
{
	const float maxFloat = std::numeric_limits<float>::max();
	Vector min(maxFloat, maxFloat, maxFloat);
	Vector max(-maxFloat, -maxFloat, -maxFloat);

	if (mesh->HasPositions()) {
		MeshData.Positions.Resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Positions[i].X = (float)mesh->mVertices[i].x;
			MeshData.Positions[i].Y = (float)mesh->mVertices[i].y;
			MeshData.Positions[i].Z = (float)mesh->mVertices[i].z;

			min = Vector::Min(min, Vector(MeshData.Positions[i].GetVector()));
			max = Vector::Max(max, Vector(MeshData.Positions[i].GetVector()));
		}
	}
	// Set bounding box for sub mesh
	AxisAlignedBoundingBox = AABox(min, max - min);

	if (mesh->HasTextureCoords(0)) {
		MeshData.TextCoords.Resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.TextCoords[i].U = (float)mesh->mTextureCoords[0][i].x;
			MeshData.TextCoords[i].V = (float)mesh->mTextureCoords[0][i].y;
		}
	}

	if (mesh->HasNormals()) {
		MeshData.Normals.Resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Normals[i].X = (float)mesh->mNormals[i].x;
			MeshData.Normals[i].Y = (float)mesh->mNormals[i].y;
			MeshData.Normals[i].Z = (float)mesh->mNormals[i].z;
		}
	}

	if (mesh->HasTangentsAndBitangents()) {
		MeshData.Tangents.Resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Tangents[i].X = (float)mesh->mTangents[i].x;
			MeshData.Tangents[i].Y = (float)mesh->mTangents[i].y;
			MeshData.Tangents[i].Z = (float)mesh->mTangents[i].z;
		}

		MeshData.Bitangents.Resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Bitangents[i].X = (float)mesh->mBitangents[i].x;
			MeshData.Bitangents[i].Y = (float)mesh->mBitangents[i].y;
			MeshData.Bitangents[i].Z = (float)mesh->mBitangents[i].z;
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
		mesh->HasNormals() ? "on" : "off",
		mesh->HasFaces() ? "on" : "off");
}

TextureResource* MeshResource::SubMesh::LoadTexture(const aiMaterial* material, const String& path, const unsigned int aiType, const eTextureUsageType textureType)
{
	TextureResource* texture = nullptr;

	aiTextureType type = (aiTextureType)aiType;
	aiString texturePath;
	if (material->GetTexture(type, 0, &texturePath) == AI_SUCCESS)
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

Poly::MeshResource::Animation::Animation(aiAnimation * anim)
{
	Duration = (float)anim->mDuration;
	TicksPerSecond = (float)anim->mTicksPerSecond;

	for (size_t i = 0; i < anim->mNumChannels; ++i)
	{
		Channel c;
		c.Name = String(anim->mChannels[i]->mNodeName.C_Str());
		for (size_t j = 0; j < anim->mChannels[i]->mNumPositionKeys; ++j)
		{
			Vector vector = { (float)anim->mChannels[i]->mPositionKeys[j].mValue.x, (float)anim->mChannels[i]->mPositionKeys[j].mValue.y, (float)anim->mChannels[i]->mPositionKeys[j].mValue.z };
			c.Positions.PushBack({ vector, (float)anim->mChannels[i]->mPositionKeys[j].mTime });
		}
		for (size_t j = 0; j < anim->mChannels[i]->mNumRotationKeys; ++j)
		{
			Quaternion q = { (float)anim->mChannels[i]->mRotationKeys[j].mValue.x, (float)anim->mChannels[i]->mRotationKeys[j].mValue.y, (float)anim->mChannels[i]->mRotationKeys[j].mValue.z, (float)anim->mChannels[i]->mRotationKeys[j].mValue.w };
			c.Rotations.PushBack({ q, (float)anim->mChannels[i]->mRotationKeys[j].mTime });
		}
		for (size_t j = 0; j < anim->mChannels[i]->mNumScalingKeys; ++j)
		{
			Vector vector = { (float)anim->mChannels[i]->mScalingKeys[j].mValue.x, (float)anim->mChannels[i]->mScalingKeys[j].mValue.y, (float)anim->mChannels[i]->mScalingKeys[j].mValue.z };
			c.Scales.PushBack({ vector, (float)anim->mChannels[i]->mScalingKeys[j].mTime });
		}
		channels.PushBack(std::move(c));
	}
}
