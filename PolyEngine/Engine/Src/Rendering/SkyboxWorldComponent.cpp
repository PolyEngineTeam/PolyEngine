#include "EnginePCH.hpp"

#include "Rendering/SkyboxWorldComponent.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/TextureResource.hpp"

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::SkyboxWorldComponent)

SkyboxWorldComponent::SkyboxWorldComponent(const String& panoramaPath, const eResourceSource source)
{
	EquirectPanorama = ResourceManager<TextureResource>::Load(panoramaPath, source, eTextureUsageType::HDR);

	gConsole.LogInfo("SkyboxWorldComponent::SkyboxWorldComponent panoramaPath: {}", panoramaPath);
}

SkyboxWorldComponent::~SkyboxWorldComponent()
{
	ResourceManager<TextureResource>::Release(EquirectPanorama);
}
