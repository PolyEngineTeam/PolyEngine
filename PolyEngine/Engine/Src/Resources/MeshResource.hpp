#pragma once

#include <Defines.hpp>
#include <Collections/Dynarray.hpp>
#include <Collections/String.hpp>
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
				Bone(String name, Matrix boneFromModel) : name(name) {}

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
				Dynarray<KeyValue<Vector>> Positions;
				Dynarray<KeyValue<Quaternion>> Rotations;
				Dynarray<KeyValue<Vector>> Scales;
			};

			String Name;
			float Duration;
			float TicksPerSecond;
			Dynarray<Channel> channels;
		};

		struct ENGINE_DLLEXPORT Bone {
			Bone(String name) : name(name) {}

			String name;
			Matrix boneFromParentBone;
			Optional<size_t> parentBoneIdx = {};
			std::vector<size_t> childrenIdx;
		};

		MeshResource(const String& path);
		virtual ~MeshResource();


		const Dynarray<SubMesh*>& GetSubMeshes() const { return SubMeshes; }
		const Dynarray<Animation*>& GetAnimations() const { return Animations; }
		const Animation* GetAnimation(const String& name) const
		{
			for (Animation* anim : Animations)
			{
				if (anim->Name == name)
					return anim;
			}
			return nullptr;
		}
		const AABox& GetAABox() const { return AxisAlignedBoundingBox; }
		const Dynarray<Bone>& GetBones() const { return Bones; }
	private:
		void LoadBones(aiNode* node);
		void PopulateBoneReferences(const std::map<String, size_t>& nameToBoneIdx, aiNode* node, const Matrix& localTransform);

		Dynarray<Bone> Bones;
		Dynarray<Animation*> Animations;
		Dynarray<SubMesh*> SubMeshes;
		AABox AxisAlignedBoundingBox;
	};
}