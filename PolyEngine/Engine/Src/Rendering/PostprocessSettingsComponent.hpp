#pragma once

#include "ECS/ComponentBase.hpp"
#include "Rendering/Camera/CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
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

	};

	REGISTER_COMPONENT(ComponentsIDGroup, PostprocessSettingsComponent)
}