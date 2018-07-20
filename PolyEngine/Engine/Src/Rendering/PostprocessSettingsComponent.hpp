#pragma once

#include "ECS/ComponentBase.hpp"
#include "Rendering/Camera/CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::PostprocessSettingsComponent) { NO_RTTI_PROPERTY(); }

		float Exposure = 1.0f;

		float MotionBlurScale = 1.0f;

		float DOFPoint = 1000.0f;
		float DOFRange = 800.0f;
		float DOFSize = 0.2f;
		float DOFShow = 0.0f;

		float BloomThreshold = 1.0f; // luminance
		float BloomScale = 1.0f;

		float GrainScale = 0.1f;
		float VignetteScale = 1.0f;
		float AbberationScale = 1.0f;

		float Gamma = 2.2f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, PostprocessSettingsComponent)
}