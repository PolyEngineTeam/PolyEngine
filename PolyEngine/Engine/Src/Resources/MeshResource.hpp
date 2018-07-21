#pragma once

#include "EnginePCH.hpp"

#include <Collections/Dynarray.hpp>
#include <Utils/EnumUtils.hpp>
#include <Math/Color.hpp>

#include "Resources/ResourceBase.hpp"
#include "Resources/TextureResource.hpp"
#include "Resources/Mesh.hpp"

struct aiMesh;
struct aiMaterial;

typedef unsigned int GLuint;
 
namespace Poly
{
	class ENGINE_DLLEXPORT MeshResource : public ResourceBase
	{
	public:
		class ENGINE_DLLEXPORT SubMesh : public BaseObject<>
		{
		public:
			SubMesh(const String& path, aiMesh* mesh, aiMaterial* material);

			void LoadGeometry(aiMesh* mesh);
			TextureResource* LoadTexture(const aiMaterial* material, const String& path, const unsigned int aiType, const eTextureUsageType textureType);

			const Mesh& GetMeshData() const { return MeshData; }
			const IMeshDeviceProxy* GetMeshProxy() const { return MeshProxy.get(); }
			const AABox& GetAABox() const { return AxisAlignedBoundingBox; }
		private:
			AABox AxisAlignedBoundingBox;
			Mesh MeshData;
			std::unique_ptr<IMeshDeviceProxy> MeshProxy;
		};

		MeshResource(const String& path);
		virtual ~MeshResource();


		const Dynarray<SubMesh*>& GetSubMeshes() const { return SubMeshes; }
		const AABox& GetAABox() const { return AxisAlignedBoundingBox; }
	private:
		Dynarray<SubMesh*> SubMeshes;
		AABox AxisAlignedBoundingBox;
	};
}