#include "EnginePCH.hpp"

#include "Rendering/SkyboxWorldComponent.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/TextureResource.hpp"

using namespace Poly;

// SkyboxWorldComponent::SkyboxWorldComponent(const EnumArray<String, eCubemapSide> cubemapPath)
// 	: Cubemap(cubemapPath)
// {
// 	gConsole.LogInfo("SkyboxWorldComponent::SkyboxWorldComponent cubemapPath: {}", cubemapPath[eCubemapSide::LEFT]);
// }

SkyboxWorldComponent::SkyboxWorldComponent(const String& panoramaPath, const eResourceSource source)
{
	EquirectPanorama = ResourceManager<TextureResource>::Load(panoramaPath, source, eTextureUsageType::HDR);

	gConsole.LogInfo("SkyboxWorldComponent::SkyboxWorldComponent panoramaPath: {}", panoramaPath);
}
