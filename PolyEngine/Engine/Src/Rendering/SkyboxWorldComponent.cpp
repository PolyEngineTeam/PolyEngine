#include "EnginePCH.hpp"

#include "Rendering/SkyboxWorldComponent.hpp"

Poly::SkyboxWorldComponent::SkyboxWorldComponent(const EnumArray<String, eCubemapSide> cubemapPath)
	: Cubemap(cubemapPath)
{
	gConsole.LogInfo("SkyboxWorldComponent::SkyboxWorldComponent cubemapPath: {}", cubemapPath[eCubemapSide::LEFT]);
}
