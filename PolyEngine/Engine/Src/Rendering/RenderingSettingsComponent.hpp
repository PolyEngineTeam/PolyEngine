#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Rendering/Camera/CameraSystem.hpp>

namespace Poly {

	enum class eShadowType
	{
		NONE = 0,
		PCF,
		EVSM2,
		EVSM4,
		_COUNT
	};

	enum class eShadowMapSize
	{
		SIZE_512 = 0,
		SIZE_1024,
		SIZE_2048,
		SIZE_4096,
		_COUNT
	};

	class ENGINE_DLLEXPORT RenderingSettingsComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::RenderingSettingsComponent) { NO_RTTI_PROPERTY(); }

		eShadowType ShadowType = eShadowType::EVSM4;
		eShadowMapSize ShadowMapSize = eShadowMapSize::SIZE_4096;
		
		float EVSMPositiveExponent = 40.0f;
		float EVSMNegativeExponent = 10.0f;
		float EVSMBias = 0.01f;
		float EVSMLghtBleedingReduction = 0.0f;

		float PCFBias = 0.01f;

		bool DebugDrawFrustumBounds = false;
		bool DebugDrawShadowCastersBounds = false;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, RenderingSettingsComponent)
}