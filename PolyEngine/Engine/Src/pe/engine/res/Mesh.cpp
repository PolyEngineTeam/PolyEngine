#include <pe/engine/EnginePCH.hpp>

#include <pe/engine/res/Mesh.hpp>
#include <pe/engine/res/ResourceManager.hpp>
#include <pe/engine/res/TextureResource.hpp>

namespace pe::engine::res {

Mesh::~Mesh()
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

}