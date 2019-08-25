#pragma once

#include <pe/Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Rendering/Camera/CameraSystem.hpp>

namespace Poly {

	class ENGINE_DLLEXPORT PostprocessSettingsComponent : public ComponentBase
	{
		friend void CameraSystem::CameraUpdatePhase(Scene*);
	public:
		RTTI_DECLARE_COMPONENT(::Poly::PostprocessSettingsComponent)
		{
			RTTI_PROPERTY( Exposure,		"Exposure",			Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( MotionBlurScale,	"MotionBlurScale",	Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( DOFPoint,		"DOFPoint",			Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( DOFRange,		"DOFRange",			Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( DOFSize,			"DOFSize",			Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( DOFShow,			"DOFShow",			Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( BloomThreshold,	"BloomThreshold",	Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( BloomScale,		"BloomScale",		Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( BloomBlurScaleX,	"BloomBlurScaleX",	Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( BloomBlurScaleY,	"BloomBlurScaleY",	Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( FogColor,		"FogColor",			Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( FogDensity,		"FogDensity",		Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( GrainScale,		"GrainScale",		Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( VignetteScale,	"VignetteScale",	Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( AbberationScale,	"AbberationScale",	Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( Tint,			"Tint",				Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( Temperature,		"Temperature",		Poly::RTTI::ePropertyFlag::NONE);
			RTTI_PROPERTY( Gamma,			"Gamma",			Poly::RTTI::ePropertyFlag::NONE);
		}

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
		core::math::Color FogColor = core::math::Color::WHITE;
		float FogDensity = 0.66f;
		float GrainScale = 0.01f;
		float VignetteScale = 0.1f;
		float AbberationScale = 0.1f;
		core::math::Color Tint = core::math::Color::WHITE;
		float Temperature = 6500.0f;
		float Gamma = 2.2f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, PostprocessSettingsComponent)
}