#include <EnginePCH.hpp>

#include <Rendering/Lighting/LightSourceComponent.hpp>

RTTI_DEFINE_COMPONENT(::Poly::AmbientLightWorldComponent)
RTTI_DEFINE_COMPONENT(::Poly::DirectionalLightComponent)
RTTI_DEFINE_COMPONENT(::Poly::PointLightComponent)
RTTI_DEFINE_COMPONENT(::Poly::SpotLightComponent)

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

Poly::SpotLightComponent::SpotLightComponent(const Color& color, float intensity, float range, float cutOff, float outerCutOff)
	: LightColor(color), Intensity(intensity), Range(range), CutOff(cutOff), OuterCutOff(outerCutOff)
{
}