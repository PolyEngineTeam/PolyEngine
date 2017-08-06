#include "EnginePCH.hpp"

#include "LightSourceComponent.hpp"

Poly::DiffuseLightSourceWorldComponent::DiffuseLightSourceWorldComponent(const Color& color, float intensity)
	: LightColor(color), Intensity(intensity)
{
}

Poly::DirectionalLightSourceComponent::DirectionalLightSourceComponent(const Color& color, float intensity)
	: LightColor(color), Intensity(intensity)
{
}

Poly::PointLightSourceComponent::PointLightSourceComponent(const Color& color, float intensity, float attenuation)
	: LightColor(color), Intensity(intensity), Attenuation(attenuation)
{
}
