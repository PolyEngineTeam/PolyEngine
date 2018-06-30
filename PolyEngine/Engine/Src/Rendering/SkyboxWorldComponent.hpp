#pragma once

#include "ECS/ComponentBase.hpp"
#include "Resources/CubemapResource.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT SkyboxWorldComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(SkyboxWorldComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY();
		}

	public:
		SkyboxWorldComponent(const EnumArray<String, eCubemapSide> cubemapPath);
		const CubemapResource& GetCubemap() const { return Cubemap; }

	private:
		CubemapResource Cubemap;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SkyboxWorldComponent)
}