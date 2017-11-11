#pragma once

#include "ComponentBase.hpp"

namespace Poly
{

	class ENGINE_DLLEXPORT AmbientLightWorldComponent : public ComponentBase
	{
	public:
		AmbientLightWorldComponent(const Color& color, float intensity);

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
		DirectionalLightComponent(const Color& color, float intensity);

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
		PointLightComponent(const Color& color, float intensity, float range);

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
}