#include "EnginePCH.hpp"
#include "Mesh.hpp"


#include "TextureResource.hpp"

Poly::Mesh::~Mesh()
{
	if (DiffuseTexture)
		ResourceManager<TextureResource>::Release(DiffuseTexture);
}
