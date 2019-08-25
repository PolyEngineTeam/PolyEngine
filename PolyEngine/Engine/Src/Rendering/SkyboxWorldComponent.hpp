#pragma once

#include <pe/Defines.hpp>
#include <ECS/ComponentBase.hpp>

namespace Poly
{
	class TextureResource;

	class ENGINE_DLLEXPORT SkyboxWorldComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::SkyboxWorldComponent) { NO_RTTI_PROPERTY(); }

		SkyboxWorldComponent(const core::storage::String& panoramaPath, const eResourceSource source);
		SkyboxWorldComponent(const std::vector<core::storage::String>& panoramaPaths, const eResourceSource source);
		~SkyboxWorldComponent();
		
		core::math::Color GetTint() const { return Tint; }
		void SetTint(core::math::Color& value) { Tint = value; }
		void SetCurrentPanorama(size_t value) { ASSERTE(value < EquirectPanoramas.size(), "SetCurrentPanorama, value out of range"); current = value; }

		const TextureResource* GetCurrentPanorama() const { return EquirectPanoramas[current]; }
		const TextureResource* GetPanorama(size_t index) const { return EquirectPanoramas[index]; }
		size_t GetCurrentIndex() const { return current; }
		size_t GetPanoramasCount() const { return EquirectPanoramas.size(); }

	private:
		std::vector<TextureResource*> EquirectPanoramas;

		core::math::Color Tint;
		size_t current = 0;

		void Init(const std::vector<core::storage::String>& panoramaPaths, const eResourceSource source);

	};

	REGISTER_COMPONENT(ComponentsIDGroup, SkyboxWorldComponent)
}