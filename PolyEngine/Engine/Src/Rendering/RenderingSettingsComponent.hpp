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

	struct EVSMSettings : public BaseObject<>
	{
		float PositiveExponent = 40.0f;
		float NegativeExponent = 10.0f;
	};

	struct PCFSettings : public BaseObject<>
	{
		float BiasMin = 0.01f;
		float BiasMax = 0.05f;
	};
	
	class ENGINE_DLLEXPORT RenderingSettingsComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::RenderingSettingsComponent) { NO_RTTI_PROPERTY(); }

		eShadowType ShadowType = eShadowType::EVSM4;
		eShadowMapSize ShadowMapSize = eShadowMapSize::SIZE_4096;
		EVSMSettings EVSM;
		PCFSettings PCF;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, RenderingSettingsComponent)
}