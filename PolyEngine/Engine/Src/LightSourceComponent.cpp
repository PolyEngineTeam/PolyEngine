#include "EnginePCH.hpp"

#include "LightSourceComponent.hpp"

Poly::AmbientLightWorldComponent::AmbientLightWorldComponent(const Color& color, float intensity)
	: LightColor(color), Intensity(intensity)
{
}

Poly::DirectionalLightComponent::DirectionalLightComponent(const Color& color, float intensity)
	: LightColor(color), Intensity(intensity)
{
}

Poly::PointLightComponent::PointLightComponent(const Color& color, float intensity, float range)
	: LightColor(color), Intensity(intensity), Range(range)
{
}

Poly::SpotLightComponent::SpotLightComponent(const Color& color = Color(1.0f, 1.0f, 1.0f), float intensity = 1.0f, float range = 10.0f, float cutOff = 15.0f)
	: LightColor(color), Intensity(intensity), Range(range), CutOff(cutOff)
{
}