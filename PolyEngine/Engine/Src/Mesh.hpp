#pragma once

#include <BaseObject.hpp>
#include <Dynarray.hpp>
#include <Vector.hpp>
#include <Color.hpp>

namespace Poly
{
	class TextureResource;

	class ENGINE_DLLEXPORT Mesh : public BaseObject<>
	{
	public:
		virtual ~Mesh();

		struct ENGINE_DLLEXPORT Vector3D { float X = 0, Y = 0, Z = 0; };
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

		const Dynarray<Vector3D>& GetPositions() const { return Positions; }
		const Dynarray<Vector3D>& GetNormals() const { return Normals; }
		const Dynarray<TextCoord>& GetTextCoords() const { return TextCoords; }
		const Dynarray<uint32_t>& GetIndicies() const { return Indices; }

		bool HasVertices() const { return Positions.GetSize() != 0; }
		bool HasNormals() const { return Normals.GetSize() != 0; }
		bool HasTextCoords() const { return TextCoords.GetSize() != 0; }
		bool HasIndicies() const { return Indices.GetSize() != 0; }

	private:
		Material Mtl;
		TextureResource* DiffuseTexture;
		Dynarray<Vector3D> Positions;
		Dynarray<Vector3D> Normals;
		Dynarray<TextCoord> TextCoords;
		Dynarray<uint32_t> Indices;

		friend class GLMeshResource;
		friend class SubMesh;
	};
}