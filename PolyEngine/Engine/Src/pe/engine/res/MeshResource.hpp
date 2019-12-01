#pragma once

#include <pe/Defines.hpp>

#include <pe/core/storage/String.hpp>
#include <pe/engine/res/ResourceBase.hpp>
#include <pe/engine/res/TextureResource.hpp>
#include <pe/engine/res/Mesh.hpp>
#include <pe/api/rendering/IRenderingDevice.hpp>

struct aiMesh;
struct aiMaterial;
struct aiAnimation;
struct aiNode;

typedef unsigned int GLuint;
 
namespace pe::engine::res 
{
	class ENGINE_DLLEXPORT MeshResource : public ResourceBase
	{
		RTTI_DECLARE_TYPE_DERIVED(MeshResource, ResourceBase)
		{
			//RTTI_PROPERTY_AUTONAME(SubMeshes, RTTI::ePropertyFlag::NONE);
		}
	public:
		class ENGINE_DLLEXPORT SubMesh : public ::pe::core::BaseObject<>
		{
		public:
			struct ENGINE_DLLEXPORT Bone {
				Bone(::pe::core::storage::String name, ::pe::core::math::Matrix boneFromModel) : name(name), boneFromModel(boneFromModel) {}

				::pe::core::storage::String name;
				::pe::core::math::Matrix boneFromModel;
			};

			SubMesh(const ::pe::core::storage::String& path, aiMesh* mesh, aiMaterial* material);

			void LoadGeometry(aiMesh* mesh);
			void LoadBones(aiMesh* mesh);
			TextureResource* LoadTexture(const aiMaterial* material, const ::pe::core::storage::String& path, const unsigned int aiType, const api::rendering::eTextureUsageType textureType);

			const Mesh& GetMeshData() const { return MeshData; }
			const api::rendering::IMeshDeviceProxy* GetMeshProxy() const { return MeshProxy.get(); }
			const ::pe::core::math::AABox& GetAABox() const { return AxisAlignedBoundingBox; }
			std::vector<Bone> GetBones() const { return Bones; }
		private:
			std::vector<Bone> Bones;
			::pe::core::math::AABox AxisAlignedBoundingBox;
			Mesh MeshData;
			std::unique_ptr<api::rendering::IMeshDeviceProxy> MeshProxy;
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

				::pe::core::storage::String Name;
				std::vector<KeyValue<::pe::core::math::Vector>> Positions;
				std::vector<KeyValue<::pe::core::math::Quaternion>> Rotations;
				std::vector<KeyValue<::pe::core::math::Vector>> Scales;
			};

			struct ChannelLerpData
			{
				std::optional<Channel::KeyValue<::pe::core::math::Vector>> pos[2];
				std::optional<Channel::KeyValue<::pe::core::math::Vector>> scale[2];
				std::optional<Channel::KeyValue<::pe::core::math::Quaternion>> rot[2];
			};

			ChannelLerpData GetLerpData(::pe::core::storage::String channel, float time) const;

			::pe::core::storage::String Name;
			float Duration;
			float TicksPerSecond;
			std::map<::pe::core::storage::String, Channel> channels;
		};

		struct ENGINE_DLLEXPORT Bone {
			Bone(::pe::core::storage::String name) : name(name) {}

			::pe::core::storage::String name;
			::pe::core::math::Matrix prevBoneFromBone;
			::pe::core::math::Matrix boneFromModel;
			std::optional<size_t> parentBoneIdx = {};
			std::vector<size_t> childrenIdx;
		};

		MeshResource(const ::pe::core::storage::String& path);
		virtual ~MeshResource();


		const std::vector<SubMesh*>& GetSubMeshes() const { return SubMeshes; }
		const Animation* GetAnimation(const ::pe::core::storage::String& name) const
		{
			auto it = Animations.find(name);
			if (it == Animations.end())
				return nullptr;
			return it->second;
		}
		const ::pe::core::math::AABox& GetAABox() const { return AxisAlignedBoundingBox; }
		const std::vector<Bone>& GetBones() const { return Bones; }

		const ::pe::core::math::Matrix& GetModelFromSkeletonRoot() const { return ModelFromSkeletonRoot; }
	private:
		void LoadBones(aiNode* node);
		void PopulateBoneReferences(const std::map<::pe::core::storage::String, size_t>& nameToBoneIdx, aiNode* node, const ::pe::core::math::Matrix& localTransform);

		::pe::core::math::Matrix ModelFromSkeletonRoot;
		std::vector<Bone> Bones;
		std::map<::pe::core::storage::String, Animation*> Animations;
		std::vector<SubMesh*> SubMeshes;
		::pe::core::math::AABox AxisAlignedBoundingBox;
	};
}