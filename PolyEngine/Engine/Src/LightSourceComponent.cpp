#include "EnginePCH.hpp"

#include "LightSourceComponent.hpp"

Poly::AmbientLightWorldComponent::AmbientLightWorldComponent(const Color& color = Color::WHITE, float intensity = 1.0f)
	: LightColor(color), Intensity(intensity)
{
}

Poly::DirectionalLightComponent::DirectionalLightComponent(const Color& color = Color::WHITE, float intensity = 1.0f)
	: LightColor(color), Intensity(intensity)
{
}

Poly::PointLightComponent::PointLightComponent(const Color& color = Color::WHITE, float intensity = 1.0f, float range = 10.0f)
	: LightColor(color), Intensity(intensity), Range(range)
{
}