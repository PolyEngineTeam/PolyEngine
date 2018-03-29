#pragma once

#include <Collections/Dynarray.hpp>
#include <Utils/EnumUtils.hpp>
#include <Math/Color.hpp>

#include "ResourceBase.hpp"
#include "TextureResource.hpp"
#include "Rendering/Mesh.hpp"

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

			const Mesh& GetMeshData() const { return MeshData; }
			const IMeshDeviceProxy* GetMeshProxy() const { return MeshProxy.get(); }
		private:
			Mesh MeshData;
			std::unique_ptr<IMeshDeviceProxy> MeshProxy;
		};

		MeshResource(const String& path);
		virtual ~MeshResource();


		const Dynarray<SubMesh*>& GetSubMeshes() const { return SubMeshes; }
	private:
		Dynarray<SubMesh*> SubMeshes;
	};
}