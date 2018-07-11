#pragma once

#include "ECS/ComponentBase.hpp"
#include "Rendering/Camera/CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(PostprocessSettingsComponent, ComponentBase)
		{
			RTTI_PROPERTY(UseBgShader, "UseBgShader", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(UseFgShader, "UseFgShader", RTTI::ePropertyFlag::NONE);

			RTTI_PROPERTY(UseCashetes, "UseCashetes", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(Distortion, "Distortion", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(ColorTempValue, "ColorTempValue", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(ColorTempLuminancePreservation, "ColorTempLuminancePreservation", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(Saturation, "Saturation", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(Grain, "Grain", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(Stripes, "Stripes", RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY(Vignette, "Vignette", RTTI::ePropertyFlag::NONE);
		}

		friend void CameraSystem::CameraUpdatePhase(World*);
	public:

		bool UseBgShader = true;
		bool UseFgShader = true;

		int UseCashetes = 0;
		float Distortion = 0.05f;
		float ColorTempValue = 7500.0f; // 6500.0 from [1000.0, 40000.0]
		float ColorTempLuminancePreservation = 0.75f;
		float Saturation = 1.0f;
		float Grain = 0.05f;
		float Stripes = 0.15f;
		float Vignette = 0.15f;
		float Exposure = 1.0f;

	};

	REGISTER_COMPONENT(ComponentsIDGroup, PostprocessSettingsComponent)
}