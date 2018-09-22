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
			SubMesh(const String& path, aiMesh* mesh, aiMaterial* material);

			struct ENGINE_DLLEXPORT Bone {
				String name;
			};

			void LoadGeometry(aiMesh* mesh);
			void LoadBones(aiMesh* mesh);
			TextureResource* LoadTexture(const aiMaterial* material, const String& path, const unsigned int aiType, const eTextureUsageType textureType);

			const Mesh& GetMeshData() const { return MeshData; }
			const IMeshDeviceProxy* GetMeshProxy() const { return MeshProxy.get(); }
			const AABox& GetAABox() const { return AxisAlignedBoundingBox; }
		private:
			AABox AxisAlignedBoundingBox;
			Mesh MeshData;
			Dynarray<Bone> Bones;
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

			float Duration;
			float TicksPerSecond;
			Dynarray<Channel> channels;
		};

		MeshResource(const String& path);
		virtual ~MeshResource();


		const Dynarray<SubMesh*>& GetSubMeshes() const { return SubMeshes; }
		const Dynarray<Animation*>& GetAnimations() const { return Animations; }
		const AABox& GetAABox() const { return AxisAlignedBoundingBox; }
	private:
		Dynarray<Animation*> Animations;
		Dynarray<SubMesh*> SubMeshes;
		AABox AxisAlignedBoundingBox;
	};
}