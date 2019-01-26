#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Rendering/Camera/CameraSystem.hpp>

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
		friend void CameraSystem::CameraUpdatePhase(Scene*);
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
		float BloomBlurScaleX = 1.0f;
		float BloomBlurScaleY = 1.0f;

		Color FogColor = Color::WHITE;
		float FogDensity = 0.66f;

		float GrainScale = 0.01f;
		float VignetteScale = 0.1f;
		float AbberationScale = 0.1f;

		Color Tint = Color::WHITE;
		float Temperature = 6500.0f;
		float Gamma = 2.2f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, PostprocessSettingsComponent)
}