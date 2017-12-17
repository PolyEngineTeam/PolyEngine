#pragma once

#include "ComponentBase.hpp"
#include "CubemapResource.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT SkyboxWorldComponent : public ComponentBase
	{
	public:
		SkyboxWorldComponent(const String cubemapPath);

	private:
		CubemapResource Cubemap;
	};

	REGISTER_COMPONENT(WorldComponentsIDGroup, SkyboxWorldComponent)
}