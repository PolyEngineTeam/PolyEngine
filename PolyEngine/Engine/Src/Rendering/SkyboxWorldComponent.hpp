#pragma once

#include <Defines.hpp>
#include "ECS/ComponentBase.hpp"

namespace Poly
{
	class TextureResource;

	class ENGINE_DLLEXPORT SkyboxWorldComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::SkyboxWorldComponent) { NO_RTTI_PROPERTY(); }

		SkyboxWorldComponent(const String& panoramaPath, const eResourceSource source);
		~SkyboxWorldComponent();
		
		Color GetTint() const { return Tint; }
		void SetTint(Color& value) { Tint = value; }

		const TextureResource* GetPanorama() const { return EquirectPanorama; }

	private:
		TextureResource* EquirectPanorama = nullptr;
		Color Tint;

	};

	REGISTER_COMPONENT(ComponentsIDGroup, SkyboxWorldComponent)
}