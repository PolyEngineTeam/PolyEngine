#include <EnginePCH.hpp>

#include <Resources/MeshResource.hpp>
#include <Resources/ResourceManager.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

using PairQueue = std::priority_queue<std::pair<u8, float>,std::vector<std::pair<u8, float>>,std::function<bool(const std::pair<u8, float>&, const std::pair<u8, float>&)>>;

RTTI_DEFINE_TYPE(Poly::MeshResource)

core::math::Matrix MatFromAiMat(const aiMatrix4x4& m)
{
	core::math::Matrix ret;
	for (int k = 0; k < 16; ++k)
		ret.Data[k] = m[k / 4][k % 4];
	return ret;
}

MeshResource::MeshResource(const core::storage::String& path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path.GetCStr(), aiProcessPreset_TargetRealtime_Fast);

	if (!scene)
	{
		core::utils::gConsole.LogError("Error Importing Asset: {}", importer.GetErrorString());
		throw ResourceLoadFailedException();
	}

	core::utils::gConsole.LogDebug("Loading model {} sucessfull.", path);

	const float maxFloat = std::numeric_limits<float>::max();
	core::math::Vector min(maxFloat, maxFloat, maxFloat);
	core::math::Vector max(-maxFloat, -maxFloat, -maxFloat);
	
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		SubMeshes.push_back(new SubMesh(path, scene->mMeshes[i], scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]));

		min = core::math::Vector::Min(min, SubMeshes[i]->GetAABox().GetMin());
		max = core::math::Vector::Max(max, SubMeshes[i]->GetAABox().GetMax());
	}

	LoadBones(scene->mRootNode);

	for (size_t i = 0; i < scene->mNumAnimations; ++i)
	{
		Animation* a = new Animation(scene->mAnimations[i]);
		Animations.emplace( a->Name, a );
	}

	AxisAlignedBoundingBox = core::math::AABox(min, max - min);
}

MeshResource::~MeshResource()
{
	for (SubMesh* subMesh : SubMeshes)
	{
		delete subMesh;
	}
	for (auto& kv : Animations)
	{
		delete kv.second;
	}
}

MeshResource::SubMesh::SubMesh(const core::storage::String& path, aiMesh* mesh, aiMaterial* material)
{
	LoadGeometry(mesh);
	LoadBones(mesh);

	MeshProxy = gEngine->GetRenderingDevice()->CreateMesh();
	MeshProxy->SetContent(MeshData);
	
	MeshData.EmissiveMap			= LoadTexture(material, path, (unsigned int)aiTextureType_EMISSIVE,		eTextureUsageType::EMISSIVE);
	MeshData.AlbedoMap				= LoadTexture(material, path, (unsigned int)aiTextureType_DIFFUSE,		eTextureUsageType::ALBEDO);
	MeshData.MetallicMap			= LoadTexture(material, path, (unsigned int)aiTextureType_SPECULAR,		eTextureUsageType::METALLIC);
	MeshData.RoughnessMap			= LoadTexture(material, path, (unsigned int)aiTextureType_SHININESS,	eTextureUsageType::ROUGHNESS);
	MeshData.NormalMap				= LoadTexture(material, path, (unsigned int)aiTextureType_NORMALS,		eTextureUsageType::NORMAL);
	MeshData.AmbientOcclusionMap	= LoadTexture(material, path, (unsigned int)aiTextureType_AMBIENT,		eTextureUsageType::AMBIENT_OCCLUSION);
}

void Poly::MeshResource::LoadBones(aiNode* node)
{
	std::map<::pe::core::storage::String, size_t> boneNameToIdx;

	ModelFromSkeletonRoot = MatFromAiMat(node->mTransformation).Inverse();

	for (const SubMesh* subMesh : SubMeshes)
	{
		for (const MeshResource::SubMesh::Bone& bone : subMesh->GetBones())
		{
			boneNameToIdx.emplace( bone.name , Bones.size());
			Bone b(bone.name);
			b.boneFromModel = bone.boneFromModel;
			Bones.push_back(b);
		}
	}

	PopulateBoneReferences(boneNameToIdx, node, core::math::Matrix::IDENTITY);
	int idx = 0;
	for (auto& bone : Bones)
	{
		core::utils::gConsole.LogDebug("[{}] Bone [{}] info:\n parent = {},\n prevBoneFromBone = {},\n boneFromModel = {}", idx, bone.name, bone.parentBoneIdx, bone.prevBoneFromBone, bone.boneFromModel);
		++idx;
	}
}

void MeshResource::PopulateBoneReferences(const std::map<::pe::core::storage::String, size_t>& nameToBoneIdx, aiNode* node, const core::math::Matrix& prevBoneFromParent)
{
	core::storage::String nodeName = node->mName.C_Str();
	core::math::Matrix parentFromBone = MatFromAiMat(node->mTransformation);
	if (nodeName.IsEmpty() || nameToBoneIdx.find(nodeName) == nameToBoneIdx.end())
	{
		for (size_t k = 0; k < node->mNumChildren; ++k)
			PopulateBoneReferences(nameToBoneIdx, node->mChildren[k], prevBoneFromParent * parentFromBone);
	}
	else
	{
		size_t idx = nameToBoneIdx.at(nodeName);
		
		if (node->mParent)
		{
			aiNode* parent = node->mParent;
			core::storage::String parentName = parent->mName.C_Str();
			while (nameToBoneIdx.find(parentName) == nameToBoneIdx.end() && parent->mParent)
			{
				parent = parent->mParent;
				parentName = parent->mName.C_Str();
			} 
			
			auto it = nameToBoneIdx.find(parentName);
			if (it != nameToBoneIdx.end())
			{
				size_t parentIdx = nameToBoneIdx.at(parentName);
				Bones[parentIdx].childrenIdx.push_back(idx);
				Bones[idx].parentBoneIdx = parentIdx;
			}
		}

		Bones[idx].prevBoneFromBone = prevBoneFromParent * parentFromBone;

		for (size_t k = 0; k < node->mNumChildren; ++k)
			PopulateBoneReferences(nameToBoneIdx, node->mChildren[k], core::math::Matrix::IDENTITY);
	}
}

void MeshResource::SubMesh::LoadBones(aiMesh* mesh)
{
	if (mesh->HasBones())
	{
		ASSERTE((i8)mesh->mNumBones <= std::numeric_limits<typename decltype(MeshData.BoneIds)::value_type::ValueType>::max(), "Model has too many bones!");
	
		std::vector<PairQueue> tmpBonesList;
		tmpBonesList.resize(mesh->mNumVertices, { PairQueue([](const std::pair<u8, float>& v1, const std::pair<u8, float>& v2) { return v1.second > v2.second; })});

		std::map<::pe::core::storage::String, size_t> nameToBoneIdx;

		for (u8 boneId = 0; boneId < mesh->mNumBones; ++boneId)
		{
			const auto& bone = mesh->mBones[boneId];
			Bones.push_back(Bone(::pe::core::storage::String(bone->mName.C_Str()), MatFromAiMat(bone->mOffsetMatrix)));
			
			if (mesh->HasPositions())
			{
				// First pass, gather bones
				for (size_t j = 0; j < bone->mNumWeights; ++j)
				{
					const auto& vertWeight = bone->mWeights[j];
					size_t vertId = vertWeight.mVertexId;
					float weight = vertWeight.mWeight;
					tmpBonesList[vertId].emplace(std::pair<u8, float>( boneId, weight ));
				}
			}
		}

		if (mesh->HasPositions())
		{
			// Apply bones
			MeshData.BoneIds.resize(mesh->mNumVertices);
			MeshData.BoneWeights.resize(mesh->mNumVertices);

			for (size_t vertId = 0; vertId < mesh->mNumVertices; ++vertId)
			{
				PairQueue& boneQueue = tmpBonesList[vertId];
				float sum = 0.f;
				for (size_t k = 0; k < 4 && boneQueue.size() > 0; ++k)
				{
					auto entry = boneQueue.top();
					boneQueue.pop();
					sum += entry.second;
					MeshData.BoneIds[vertId].Data[k] = entry.first;
					MeshData.BoneWeights[vertId].Data[k] = entry.second;
				}
				//ASSERTE(sum >= 0.90f, "Detected >10% animation inaccuracy (too many weights for one vertex)");
			}
		}

		core::utils::gConsole.LogDebug("{} bones loaded", mesh->mNumBones);
	}
	else
	{
		core::utils::gConsole.LogWarning("No bones");
	}
}

void MeshResource::SubMesh::LoadGeometry(aiMesh* mesh)
{
	const float maxFloat = std::numeric_limits<float>::max();
	core::math::Vector min(maxFloat, maxFloat, maxFloat);
	core::math::Vector max(-maxFloat, -maxFloat, -maxFloat);

	if (mesh->HasPositions()) {
		MeshData.Positions.resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Positions[i].X = (float)mesh->mVertices[i].x;
			MeshData.Positions[i].Y = (float)mesh->mVertices[i].y;
			MeshData.Positions[i].Z = (float)mesh->mVertices[i].z;

			min = core::math::Vector::Min(min, core::math::Vector(MeshData.Positions[i].GetVector()));
			max = core::math::Vector::Max(max, core::math::Vector(MeshData.Positions[i].GetVector()));
		}
	}
	// Set bounding box for sub mesh
	AxisAlignedBoundingBox = core::math::AABox(min, max - min);

	if (mesh->HasTextureCoords(0)) {
		MeshData.TextCoords.resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.TextCoords[i].U = (float)mesh->mTextureCoords[0][i].x;
			MeshData.TextCoords[i].V = (float)mesh->mTextureCoords[0][i].y;
		}
	}

	if (mesh->HasNormals()) {
		MeshData.Normals.resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Normals[i].X = (float)mesh->mNormals[i].x;
			MeshData.Normals[i].Y = (float)mesh->mNormals[i].y;
			MeshData.Normals[i].Z = (float)mesh->mNormals[i].z;
		}
	}

	if (mesh->HasTangentsAndBitangents()) {
		MeshData.Tangents.resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Tangents[i].X = (float)mesh->mTangents[i].x;
			MeshData.Tangents[i].Y = (float)mesh->mTangents[i].y;
			MeshData.Tangents[i].Z = (float)mesh->mTangents[i].z;
		}

		MeshData.Bitangents.resize(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			MeshData.Bitangents[i].X = (float)mesh->mBitangents[i].x;
			MeshData.Bitangents[i].Y = (float)mesh->mBitangents[i].y;
			MeshData.Bitangents[i].Z = (float)mesh->mBitangents[i].z;
		}
	}

	if (mesh->HasFaces()) {
		MeshData.Indices.resize(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			MeshData.Indices[i * 3] = mesh->mFaces[i].mIndices[0];
			MeshData.Indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			MeshData.Indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}
	}
	
	core::utils::gConsole.LogDebug(
		"Loaded mesh entry: {} with {} vertices, {} faces and parameters: "
		"pos[{}], tex_coord[{}], norm[{}], faces[{}]",
		mesh->mName.C_Str(), MeshData.Positions.size(), mesh->mNumFaces,
		mesh->HasPositions() ? "on" : "off",
		mesh->HasTextureCoords(0) ? "on" : "off",
		mesh->HasNormals() ? "on" : "off",
		mesh->HasFaces() ? "on" : "off");
}

TextureResource* MeshResource::SubMesh::LoadTexture(const aiMaterial* material, const core::storage::String& path, const unsigned int aiType, const eTextureUsageType textureType)
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
		core::storage::String textPath(fullPath.c_str());
		// end temporary code for extracting path

		texture = ResourceManager<TextureResource>::Load(textPath, eResourceSource::NONE, textureType);
		if (!texture) {
			core::utils::gConsole.LogError("Failed to load texture: {}", textPath);
		}
		else {
			core::utils::gConsole.LogDebug("Succeded to load texture: {}", textPath);
		}
	}
	else {
		core::utils::gConsole.LogError("Failed to load texture for material: {}", path);
	}

	return texture;
}

Poly::MeshResource::Animation::Animation(aiAnimation * anim)
{
	Duration = (float)anim->mDuration;
	TicksPerSecond = (float)anim->mTicksPerSecond;
	Name = core::storage::String(anim->mName.C_Str());

	for (size_t i = 0; i < anim->mNumChannels; ++i)
	{
		Channel c;
		c.Name = core::storage::String(anim->mChannels[i]->mNodeName.C_Str());
		for (size_t j = 0; j < anim->mChannels[i]->mNumPositionKeys; ++j)
		{
			core::math::Vector vector = { (float)anim->mChannels[i]->mPositionKeys[j].mValue.x, (float)anim->mChannels[i]->mPositionKeys[j].mValue.y, (float)anim->mChannels[i]->mPositionKeys[j].mValue.z };
			c.Positions.push_back({ vector, (float)anim->mChannels[i]->mPositionKeys[j].mTime });
		}
		for (size_t j = 0; j < anim->mChannels[i]->mNumRotationKeys; ++j)
		{
			core::math::Quaternion q = { (float)anim->mChannels[i]->mRotationKeys[j].mValue.x, (float)anim->mChannels[i]->mRotationKeys[j].mValue.y, (float)anim->mChannels[i]->mRotationKeys[j].mValue.z, (float)anim->mChannels[i]->mRotationKeys[j].mValue.w };
			c.Rotations.push_back({ q, (float)anim->mChannels[i]->mRotationKeys[j].mTime });
		}
		for (size_t j = 0; j < anim->mChannels[i]->mNumScalingKeys; ++j)
		{
			core::math::Vector vector = { (float)anim->mChannels[i]->mScalingKeys[j].mValue.x, (float)anim->mChannels[i]->mScalingKeys[j].mValue.y, (float)anim->mChannels[i]->mScalingKeys[j].mValue.z };
			c.Scales.push_back({ vector, (float)anim->mChannels[i]->mScalingKeys[j].mTime });
		}
		core::storage::String nameCpy = c.Name;
		channels.emplace( nameCpy, std::move(c) );
	}
}

Poly::MeshResource::Animation::ChannelLerpData Poly::MeshResource::Animation::GetLerpData(::pe::core::storage::String channelName, float time) const
{
	ChannelLerpData data;

	// @todo(muniu) investigate TicksPerSecond usage.
	size_t hintIdx = 0;// (size_t)(TicksPerSecond * time);

	const auto& channel = channels.at(channelName);

	for (size_t i = hintIdx; i < channel.Positions.size(); ++i)
	{
		auto& pos = channel.Positions[i];
		if (pos.Time <= time)
			data.pos[0] = pos;
		if (pos.Time >= time)
		{
			data.pos[1] = pos;
			break;
		}
	}

	for (size_t i = hintIdx; i < channel.Positions.size(); ++i)
	{
		auto& scale = channel.Scales[i];
		if (scale.Time <= time)
			data.scale[0] = scale;
		if (scale.Time >= time)
		{
			data.scale[1] = scale;
			break;
		}
	}

	for (size_t i = hintIdx; i < channel.Positions.size(); ++i)
	{
		auto& rot = channel.Rotations[i];
		if (rot.Time <= time)
			data.rot[0] = rot;
		if (rot.Time >= time)
		{
			data.rot[1] = rot;
			break;
		}
	}

	return data;
}
