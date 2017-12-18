#pragma once

#include "ComponentBase.hpp"
#include "CubemapResource.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT SkyboxWorldComponent : public ComponentBase
	{
	public:
		SkyboxWorldComponent(const String cubemapPath);
		const CubemapResource& GetCubemap() const { return Cubemap; }

	private:
		CubemapResource Cubemap;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, SkyboxWorldComponent)
}