#pragma once

#include <pe/Defines.hpp>
#include "ECS/ComponentBase.hpp"
#include "pe/core/math/Frustum.hpp"
#include "pe/core/math/Matrix.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT AmbientLightWorldComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::AmbientLightWorldComponent) { NO_RTTI_PROPERTY(); }

		AmbientLightWorldComponent(const core::math::Color& color = core::math::Color::WHITE, float intensity = 1.0f);

		const core::math::Color& GetColor() const { return LightColor; }
		void SetColor(const core::math::Color& value) { LightColor = value; }
		float GetIntensity() const { return Intensity; }
		void SetIntensity(float value) { Intensity = value; }

	private:
		core::math::Color LightColor;
		float Intensity = 1.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, AmbientLightWorldComponent)

	class ENGINE_DLLEXPORT DirectionalLightComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::DirectionalLightComponent) { NO_RTTI_PROPERTY(); }

		DirectionalLightComponent(const core::math::Color& color = core::math::Color::WHITE, float intensity = 1.0f);

		const core::math::Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }

	private:
		core::math::Color LightColor;
		float Intensity = 1.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, DirectionalLightComponent)

	class ENGINE_DLLEXPORT PointLightComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::PointLightComponent) { NO_RTTI_PROPERTY(); }

		PointLightComponent(const core::math::Color& color = core::math::Color::WHITE, float intensity = 1.0f, float range = 10.0f);

		const core::math::Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }
		void SetIntensity(float value) { Intensity = value; }
		void SetRange(float value) { Range = value; }
		float GetRange() const { return Range; }
	private:
		core::math::Color LightColor;
		float Intensity = 1.0f;
		float Range = 1.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, PointLightComponent)

	class ENGINE_DLLEXPORT SpotLightComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::SpotLightComponent) { NO_RTTI_PROPERTY(); }

		SpotLightComponent(const core::math::Color& color = core::math::Color::WHITE, float intensity = 1.0f, float range = 10.0f, float cutoff = 12.0f, float outerCutOff = 17.0f);
		const core::math::Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }
		void SetRange(float value) { Range = value; }
		float GetRange() const { return Range; }
		void SetCutOff(float value) { CutOff = value; }
		float GetCutOff() const { return CutOff; }
		void SetOuterCutOff(float value) { OuterCutOff = value; }
		float GetOuterCutOff() const { return OuterCutOff; }
	private:
		core::math::Color LightColor;
		float Intensity = 1.0f;
		float Range = 1.0f;
		float CutOff = 15.0f;
		float OuterCutOff = 15.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SpotLightComponent)
}