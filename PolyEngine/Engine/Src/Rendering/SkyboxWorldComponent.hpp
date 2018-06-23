#pragma once

#include "ECS/ComponentBase.hpp"

namespace Poly
{
	class TextureResource;
	class CubemapResource;

	class ENGINE_DLLEXPORT SkyboxWorldComponent : public ComponentBase
	{
	public:
		// SkyboxWorldComponent(const EnumArray<String, eCubemapSide> cubemapPath);
		SkyboxWorldComponent(const String& panoramaPath, const eResourceSource source);
		
		const CubemapResource* GetCubemap() const { return Cubemap; }
		const TextureResource* GetPanorama() const { return EquirectPanorama; }

	private:
		TextureResource* EquirectPanorama = nullptr;
		CubemapResource* Cubemap = nullptr;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, SkyboxWorldComponent)
}