#pragma once

#include "ComponentBase.hpp"

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

	REGISTER_COMPONENT(WorldComponentsIDGroup, AmbientLightWorldComponent)

	class ENGINE_DLLEXPORT DirectionalLightComponent : public ComponentBase
	{
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
	public:
		PointLightComponent(const Color& color = Color::WHITE, float intensity = 1.0f, float range = 10.0f);

		const Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }
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
	public:
		SpotLightComponent(const Color& color = Color::WHITE, float intensity = 1.0f, float range = 10.0f, float cutoff = 12.0f);

		const Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }
		void SetRange(float value) { Range = value; }
		float GetRange() const { return Range; }
		void SetCutOff(float value) { CutOff = value; }
		float GetCutOff() const { return CutOff; }
	private:
		Color LightColor;
		float Intensity = 1.0f;
		float Range = 1.0f;
		float CutOff = 15.0f;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SpotLightComponent)
}