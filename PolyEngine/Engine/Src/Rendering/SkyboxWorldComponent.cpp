#include "EnginePCH.hpp"

#include "Rendering/SkyboxWorldComponent.hpp"

RTTI_DEFINE_TYPE(Poly::SkyboxWorldComponent)

Poly::SkyboxWorldComponent::SkyboxWorldComponent(const EnumArray<String, eCubemapSide> cubemapPath)
	: Cubemap(cubemapPath)
{
	gConsole.LogInfo("SkyboxWorldComponent::SkyboxWorldComponent cubemapPath: {}", cubemapPath[eCubemapSide::LEFT]);
}
