#pragma once

#include "ECS/ComponentBase.hpp"
#include "Rendering/Camera/CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::PostprocessSettingsComponent) { NO_RTTI_PROPERTY(); }

		float Exposure = 1.0f;

		float MotionBlurScale = 0.1f;

		float DOFPoint = 1000.0f;
		float DOFRange = 800.0f;
		float DOFSize = 0.1f;
		float DOFShow = 0.0f;

		float BloomThreshold = 1.0f; // luminance
		float BloomScale = 0.1f;

		float GrainScale = 0.01f;
		float VignetteScale = 0.1f;
		float AbberationScale = 0.1f;

		Color Tint = Color::WHITE;
		float Gamma = 2.2f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, PostprocessSettingsComponent)
}