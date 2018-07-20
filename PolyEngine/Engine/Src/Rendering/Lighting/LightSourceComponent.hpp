#pragma once

#include "ECS/ComponentBase.hpp"

namespace Poly
{

	class ENGINE_DLLEXPORT AmbientLightWorldComponent : public ComponentBase
	{
	public:
		AmbientLightWorldComponent(const Color& color = Color::WHITE, float intensity = 1.0f);

		const Color& GetColor() const { return LightColor; }
		void SetColor(const Color& value) { LightColor = value; }
		float GetIntensity() const { return Intensity; }
		void SetIntensity(float value) { Intensity = value; }

	private:
		Color LightColor;
		float Intensity = 1.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, AmbientLightWorldComponent)

	class ENGINE_DLLEXPORT DirectionalLightComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(DirectionalLightComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY()
		}

	public:
		DirectionalLightComponent(const Color& color = Color::WHITE, float intensity = 1.0f);

		const Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }
	private:
		Color LightColor;
		float Intensity = 1.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, DirectionalLightComponent)

	class ENGINE_DLLEXPORT PointLightComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(PointLightComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY()
		}

	public:
		PointLightComponent(const Color& color = Color::WHITE, float intensity = 1.0f, float range = 10.0f);

		const Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }
		void SetIntensity(float value) { Intensity = value; }
		void SetRange(float value) { Range = value; }
		float GetRange() const { return Range; }
	private:
		Color LightColor;
		float Intensity = 1.0f;
		float Range = 1.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, PointLightComponent)

	class ENGINE_DLLEXPORT SpotLightComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(SpotLightComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY()
		}

	public:
		SpotLightComponent(const Color& color = Color::WHITE, float intensity = 1.0f, float range = 10.0f, float cutoff = 12.0f, float outerCutOff = 17.0f);

		const Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }
		void SetRange(float value) { Range = value; }
		float GetRange() const { return Range; }
		void SetCutOff(float value) { CutOff = value; }
		float GetCutOff() const { return CutOff; }
		void SetOuterCutOff(float value) { OuterCutOff = value; }
		float GetOuterCutOff() const { return OuterCutOff; }
	private:
		Color LightColor;
		float Intensity = 1.0f;
		float Range = 1.0f;
		float CutOff = 15.0f;
		float OuterCutOff = 15.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SpotLightComponent)
}