#pragma once

#include <BaseObject.hpp>
#include <Dynarray.hpp>
#include "Vector3f.hpp"
#include <Color.hpp>

namespace Poly
{
	class TextureResource;

	class ENGINE_DLLEXPORT Mesh : public BaseObject<>
	{
	public:
		virtual ~Mesh();

		struct ENGINE_DLLEXPORT TextCoord { float U = 0, V = 0; };
		struct ENGINE_DLLEXPORT Material
		{
			float SpecularIntensity;
			float SpecularPower;
			Color SpecularColor;
		};

		
		const TextureResource* GetDiffTexture() const { return DiffuseTexture; }
		const Material& GetMaterial() { return Mtl; }
		size_t GetVertexCount() const { return Positions.GetSize(); }
		size_t GetTriangleCount() const { return Indices.GetSize() / 3; }

		const Dynarray<Vector3f>& GetPositions() const { return Positions; }
		const Dynarray<Vector3f>& GetNormals() const { return Normals; }
		const Dynarray<TextCoord>& GetTextCoords() const { return TextCoords; }
		const Dynarray<uint32_t>& GetIndicies() const { return Indices; }

		bool HasVertices() const { return Positions.GetSize() != 0; }
		bool HasNormals() const { return Normals.GetSize() != 0; }
		bool HasTextCoords() const { return TextCoords.GetSize() != 0; }
		bool HasIndicies() const { return Indices.GetSize() != 0; }

	private:
		Material Mtl;
		TextureResource* DiffuseTexture;
		Dynarray<Vector3f> Positions;
		Dynarray<Vector3f> Normals;
		Dynarray<TextCoord> TextCoords;
		Dynarray<uint32_t> Indices;

		friend class MeshResource;
		friend class SubMesh;
	};
}