#pragma once

#include "ECS/ComponentBase.hpp"
#include "Rendering/Camera/CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
	public:

		// bool UseBgShader = true;
		// bool UseFgShader = true;

		// int UseCashetes = 0;
		// float Distortion = 0.05f;
		// float ColorTempValue = 7500.0f; // 6500.0 from [1000.0, 40000.0]
		// float ColorTempLuminancePreservation = 0.75f;
		// float Saturation = 1.0f;
		// float Grain = 0.05f;
		// float Stripes = 0.15f;
		// float Vignette = 0.15f;
		float Exposure = 1.0f;
		float DOFPoint = 1000.0f;
		float DOFRange = 800.0f;
		float DOFSize = 0.2f;
		float DOFShow = 0.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, PostprocessSettingsComponent)
}