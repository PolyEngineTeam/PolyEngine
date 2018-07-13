#pragma once

#include "ECS/ComponentBase.hpp"

namespace Poly
{
	class TextureResource;

	class ENGINE_DLLEXPORT SkyboxWorldComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(SkyboxWorldComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY();
		}

	public:
		SkyboxWorldComponent(const String& panoramaPath, const eResourceSource source);
		~SkyboxWorldComponent();
		
		const TextureResource* GetPanorama() const { return EquirectPanorama; }

	private:
		TextureResource* EquirectPanorama = nullptr;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SkyboxWorldComponent)
}