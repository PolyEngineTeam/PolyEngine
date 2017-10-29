#pragma once

#include "ComponentBase.hpp"

namespace Poly
{

	class ENGINE_DLLEXPORT DiffuseLightSourceWorldComponent : public ComponentBase
	{
	public:
		DiffuseLightSourceWorldComponent(const Color& color, float intensity);

		const Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }
	private:
		Color LightColor;
		float Intensity = 1.0f;
	};

	COMPONENT_INS(WorldComponentsFamily, DiffuseLightSourceWorldComponent)

	class ENGINE_DLLEXPORT DirectionalLightSourceComponent : public ComponentBase
	{
	public:
		DirectionalLightSourceComponent(const Color& color, float intensity);

		const Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }
	private:
		Color LightColor;
		float Intensity = 1.0f;
	};

	class ENGINE_DLLEXPORT PointLightSourceComponent : public ComponentBase
	{
	public:
		PointLightSourceComponent(const Color& color, float intensity, float attenuation);

		const Color& GetColor() const { return LightColor; }
		float GetIntensity() const { return Intensity; }
		float GetAttenuation() const { return Attenuation; }
	private:
		Color LightColor;
		float Intensity = 1.0f;
		float Attenuation = 1.0f;
	};
}