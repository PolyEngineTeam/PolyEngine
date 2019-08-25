#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/Vector3f.hpp>
#include <pe/core/math/VectorT.hpp>

namespace Poly
{
	class TextureResource;

	class ENGINE_DLLEXPORT Mesh : public ::pe::core::BaseObject<>
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
		size_t GetVertexCount() const { return Positions.size(); }
		size_t GetTriangleCount() const { return Indices.size() / 3; }

		const std::vector<core::math::Vector3f>& GetPositions() const { return Positions; }
		const std::vector<core::math::Vector3f>& GetNormals() const { return Normals; }
		const std::vector<core::math::Vector3f>& GetTangents() const { return Tangents; }
		const std::vector<core::math::Vector3f>& GetBitangents() const { return Bitangents; }
		const std::vector<TextCoord>& GetTextCoords() const { return TextCoords; }
		const std::vector<uint32_t>& GetIndicies() const { return Indices; }
		const std::vector<core::math::VectorT<i8, 4>>& GetBoneIds() const { return BoneIds; }
		const std::vector<core::math::VectorT<float, 4>>& GetBoneWeights() const { return BoneWeights; }

		bool HasVertices() const { return Positions.size() != 0; }
		bool HasNormals() const { return Normals.size() != 0; }
		bool HasTangents() const { return Tangents.size() != 0; }
		bool HasBitangents() const { return Bitangents.size() != 0; }
		bool HasTextCoords() const { return TextCoords.size() != 0; }
		bool HasIndicies() const { return Indices.size() != 0; }
		bool HasBones() const { return BoneIds.size() != 0 && BoneWeights.size() != 0; }
		bool HasBoneWeights() const { return BoneWeights.size() != 0; }
		bool HasBoneIds() const { return BoneIds.size() != 0; }

	private:
		TextureResource* AlbedoMap;
		TextureResource* RoughnessMap;
		TextureResource* MetallicMap;
		TextureResource* AmbientOcclusionMap;
		TextureResource* NormalMap;
		TextureResource* EmissiveMap;
		std::vector<core::math::Vector3f> Positions;
		std::vector<core::math::Vector3f> Normals;
		std::vector<core::math::Vector3f> Tangents;
		std::vector<core::math::Vector3f> Bitangents;
		std::vector<TextCoord> TextCoords;
		std::vector<uint32_t> Indices;
		std::vector<core::math::VectorT<i8, 4>> BoneIds; // Max 4 bones
		std::vector<core::math::VectorT<float, 4>> BoneWeights;
		friend class MeshResource;
		friend class SubMesh;
	};
}