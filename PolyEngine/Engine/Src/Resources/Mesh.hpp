#pragma once

#include <Defines.hpp>
#include <Collections/Dynarray.hpp>
#include <Math/Vector3f.hpp>

namespace Poly
{
	class TextureResource;

	class ENGINE_DLLEXPORT Mesh : public BaseObject<>
	{
	public:
		virtual ~Mesh();

		struct ENGINE_DLLEXPORT TextCoord { float U = 0, V = 0; };

		const TextureResource* GetAlbedoMap() const { return AlbedoMap; }
		const TextureResource* GetRoughnessMap() const { return RoughnessMap; }
		const TextureResource* GetMetallicMap() const { return MetallicMap; }
		const TextureResource* GetAmbientOcclusionMap() const { return AmbientOcclusionMap; }
		const TextureResource* GetNormalMap() const { return NormalMap; }
		const TextureResource* GetEmissiveMap() const { return EmissiveMap; }
		size_t GetVertexCount() const { return Positions.GetSize(); }
		size_t GetTriangleCount() const { return Indices.GetSize() / 3; }

		const Dynarray<Vector3f>& GetPositions() const { return Positions; }
		const Dynarray<Vector3f>& GetNormals() const { return Normals; }
		const Dynarray<Vector3f>& GetTangents() const { return Tangents; }
		const Dynarray<Vector3f>& GetBitangents() const { return Bitangents; }
		const Dynarray<TextCoord>& GetTextCoords() const { return TextCoords; }
		const Dynarray<uint32_t>& GetIndicies() const { return Indices; }

		bool HasVertices() const { return Positions.GetSize() != 0; }
		bool HasNormals() const { return Normals.GetSize() != 0; }
		bool HasTangents() const { return Tangents.GetSize() != 0; }
		bool HasBitangents() const { return Bitangents.GetSize() != 0; }
		bool HasTextCoords() const { return TextCoords.GetSize() != 0; }
		bool HasIndicies() const { return Indices.GetSize() != 0; }

	private:
		TextureResource* AlbedoMap;
		TextureResource* RoughnessMap;
		TextureResource* MetallicMap;
		TextureResource* AmbientOcclusionMap;
		TextureResource* NormalMap;
		TextureResource* EmissiveMap;
		Dynarray<Vector3f> Positions;
		Dynarray<Vector3f> Normals;
		Dynarray<Vector3f> Tangents;
		Dynarray<Vector3f> Bitangents;
		Dynarray<TextCoord> TextCoords;
		Dynarray<uint32_t> Indices;

		friend class MeshResource;
		friend class SubMesh;
	};
}