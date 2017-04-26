#include "EnginePCH.hpp"

#include "CameraComponent.hpp"

using namespace Poly;

BaseCameraComponent::BaseCameraComponent(float fov,  float near, float far)
	: IsPerspective(true), Fov(fov), Near(near), Far(far) 
{
}

BaseCameraComponent::BaseCameraComponent(float top, float bottom, float near, float far)
	: IsPerspective(false), Top(top), Bottom(bottom), Near(near), Far(far)
{
	ASSERTE(false, "Orthographics projection is not yet implemented");
}
