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

Poly::PointLightComponent::PointLightComponent(const Color& color, float intensity, float attenuation)
	: LightColor(color), Intensity(intensity), Attenuation(attenuation)
{
}