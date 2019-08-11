#pragma once

#include <pe/Defines.hpp>

#include <pe/core/storage/String.hpp>
#include <Resources/ResourceBase.hpp>
#include <Resources/TextureResource.hpp>
#include <Resources/Mesh.hpp>
#include <Rendering/IRenderingDevice.hpp>

struct aiMesh;
struct aiMaterial;
struct aiAnimation;
struct aiNode;

typedef unsigned int GLuint;
 
namespace Poly
{
	class ENGINE_DLLEXPORT MeshResource : public ResourceBase
	{
		RTTI_DECLARE_TYPE_DERIVED(MeshResource, ResourceBase)
		{
			//RTTI_PROPERTY_AUTONAME(SubMeshes, RTTI::ePropertyFlag::NONE);
		}
	public:
		class ENGINE_DLLEXPORT SubMesh : public BaseObject<>
		{
		public:
			struct ENGINE_DLLEXPORT Bone {
				Bone(String name, Matrix boneFromModel) : name(name), boneFromModel(boneFromModel) {}

				String name;
				Matrix boneFromModel;
			};

			SubMesh(const String& path, aiMesh* mesh, aiMaterial* material);

			void LoadGeometry(aiMesh* mesh);
			void LoadBones(aiMesh* mesh);
			TextureResource* LoadTexture(const aiMaterial* material, const String& path, const unsigned int aiType, const eTextureUsageType textureType);

			const Mesh& GetMeshData() const { return MeshData; }
			const IMeshDeviceProxy* GetMeshProxy() const { return MeshProxy.get(); }
			const AABox& GetAABox() const { return AxisAlignedBoundingBox; }
			std::vector<Bone> GetBones() const { return Bones; }
		private:
			std::vector<Bone> Bones;
			AABox AxisAlignedBoundingBox;
			Mesh MeshData;
			std::unique_ptr<IMeshDeviceProxy> MeshProxy;
		};

		struct ENGINE_DLLEXPORT Animation {

			Animation(aiAnimation* anim);

			struct ENGINE_DLLEXPORT Channel {
				template<typename T>
				struct KeyValue
				{
					T Value;
					float Time;
				};

				String Name;
				std::vector<KeyValue<Vector>> Positions;
				std::vector<KeyValue<Quaternion>> Rotations;
				std::vector<KeyValue<Vector>> Scales;
			};

			struct ChannelLerpData
			{
				std::optional<Channel::KeyValue<Vector>> pos[2];
				std::optional<Channel::KeyValue<Vector>> scale[2];
				std::optional<Channel::KeyValue<Quaternion>> rot[2];
			};

			ChannelLerpData GetLerpData(String channel, float time) const;

			String Name;
			float Duration;
			float TicksPerSecond;
			std::map<String, Channel> channels;
		};

		struct ENGINE_DLLEXPORT Bone {
			Bone(String name) : name(name) {}

			String name;
			Matrix prevBoneFromBone;
			Matrix boneFromModel;
			std::optional<size_t> parentBoneIdx = {};
			std::vector<size_t> childrenIdx;
		};

		MeshResource(const String& path);
		virtual ~MeshResource();


		const std::vector<SubMesh*>& GetSubMeshes() const { return SubMeshes; }
		const Animation* GetAnimation(const String& name) const
		{
			auto it = Animations.find(name);
			if (it == Animations.end())
				return nullptr;
			return it->second;
		}
		const AABox& GetAABox() const { return AxisAlignedBoundingBox; }
		const std::vector<Bone>& GetBones() const { return Bones; }

		const Matrix& GetModelFromSkeletonRoot() const { return ModelFromSkeletonRoot; }
	private:
		void LoadBones(aiNode* node);
		void PopulateBoneReferences(const std::map<String, size_t>& nameToBoneIdx, aiNode* node, const Matrix& localTransform);

		Matrix ModelFromSkeletonRoot;
		std::vector<Bone> Bones;
		std::map<String, Animation*> Animations;
		std::vector<SubMesh*> SubMeshes;
		AABox AxisAlignedBoundingBox;
	};
}