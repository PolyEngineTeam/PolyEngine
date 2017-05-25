#pragma once

#include <Dynarray.hpp>
#include <EnumUtils.hpp>
#include <Color.hpp>

#include "ResourceBase.hpp"
#include "GLTextureResource.hpp"

struct aiMesh;
struct aiMaterial;

typedef unsigned int GLuint;
 
namespace Poly
{
	class ENGINE_DLLEXPORT GLMeshResource : public ResourceBase
	{
	public:
		class ENGINE_DLLEXPORT SubMesh : public BaseObject<>
		{
		public:
			enum class eBufferType {
				VERTEX_BUFFER,
				TEXCOORD_BUFFER,
				NORMAL_BUFFER,
				INDEX_BUFFER,
				_COUNT
			};

			struct Material {
				float SpecularIntensity;
				float SpecularPower;
				Color SpecularColor;
			};
			
			SubMesh(const String& path, aiMesh* mesh, aiMaterial* material);
			virtual ~SubMesh();

			GLuint GetVAO() const { return VAO; }
			GLuint GetVBO(eBufferType type) const { return VBO[type]; }
			const GLTextureResource* GetDiffTexture() const { return DiffuseTexture; }
			size_t GetVertexCount() const { return VertexCount; }
			const Material& GetMaterial() { return Mtl; }

		private:
			GLuint VAO;
			EnumArray<GLuint, eBufferType> VBO;
			GLTextureResource* DiffuseTexture = nullptr; // only diffuse texture for now
			size_t VertexCount;
			Material Mtl;
		};

		GLMeshResource(const String& path);
		virtual ~GLMeshResource();


		const Dynarray<SubMesh*>& GetSubMeshes() const { return SubMeshes; }
	private:
		Dynarray<SubMesh*> SubMeshes;
	};
}