#include "EnginePCH.hpp"
#include "Resources/Mesh.hpp"


#include "Resources/TextureResource.hpp"

Poly::Mesh::~Mesh()
{
	if (DiffuseTexture)
		ResourceManager<TextureResource>::Release(DiffuseTexture);

	if (SpecularMap)
		ResourceManager<TextureResource>::Release(SpecularMap);

	if(NormalMap)
		ResourceManager<TextureResource>::Release(NormalMap);
}
