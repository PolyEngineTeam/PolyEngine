#pragma once

#include "ComponentBase.hpp"
#include "CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
		friend void CameraSystem::CameraUpdatePhase(World*);

	public:
	
		int UseCashetes = 1;
		float Distortion = 0.1f;
		float Bloom = 0.1f;
		float BloomThreshold = 0.7f;
		float ColorTempValue = 7000.0f; // 6500.0 from [1000.0, 40000.0]
		float ColorTempLuminancePreservation = 0.75f;
		float Saturation = 1.0f;
		float Grain = 0.0f;
		float Stripes = 0.0f;
		float Vignette = 0.0f;
		float Brightness = 0.0;
		float Contrast = 1.0;
	};
}