#include "EnginePCH.hpp"

#include "SkyboxWorldComponent.hpp"

Poly::SkyboxWorldComponent::SkyboxWorldComponent(const Dynarray<String> cubemapPath)
	: Cubemap(cubemapPath)
{
	gConsole.LogInfo("SkyboxWorldComponent::SkyboxWorldComponent cubemapPath: {}", cubemapPath[0]);
}
