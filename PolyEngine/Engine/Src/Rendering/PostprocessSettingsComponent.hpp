#pragma once

#include "ECS/ComponentBase.hpp"
#include "Rendering/Camera/CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(PostprocessSettingsComponent, ComponentBase)
		{
			RTTI_PROPERTY_AUTONAME(Exposure, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(MotionBlurScale, RTTI::ePropertyFlag::NONE);

			RTTI_PROPERTY_AUTONAME(DOFPoint, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(DOFRange, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(DOFSize, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(DOFShow, RTTI::ePropertyFlag::NONE);

			RTTI_PROPERTY_AUTONAME(BloomThreshold, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(BloomScale, RTTI::ePropertyFlag::NONE);

			RTTI_PROPERTY_AUTONAME(GrainScale, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(VignetteScale, RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY_AUTONAME(AbberationScale, RTTI::ePropertyFlag::NONE);

			RTTI_PROPERTY_AUTONAME(Gamma, RTTI::ePropertyFlag::NONE);
		}

		friend void CameraSystem::CameraUpdatePhase(Scene*);
	public:

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