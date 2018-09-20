#include <EnginePCH.hpp>

#include <Resources/Mesh.hpp>
#include <Resources/ResourceManager.hpp>
#include <Resources/TextureResource.hpp>

Poly::Mesh::~Mesh()
{
	if (AlbedoMap)
		ResourceManager<TextureResource>::Release(AlbedoMap);

	if (RoughnessMap)
		ResourceManager<TextureResource>::Release(RoughnessMap);

	if (MetallicMap)
		ResourceManager<TextureResource>::Release(MetallicMap);

	if (AmbientOcclusionMap)
		ResourceManager<TextureResource>::Release(AmbientOcclusionMap);

	if (NormalMap)
		ResourceManager<TextureResource>::Release(NormalMap);

	if (EmissiveMap)
		ResourceManager<TextureResource>::Release(EmissiveMap);
}
