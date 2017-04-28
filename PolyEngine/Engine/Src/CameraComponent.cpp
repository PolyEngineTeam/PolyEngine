#include "EnginePCH.hpp"

using namespace Poly;

CameraComponent::CameraComponent(float fov,  float zNear, float zFar)
	: IsPerspective(true), Fov(fov), Near(zNear), Far(zFar)
{
}

CameraComponent::CameraComponent(float top, float bottom, float zNear, float zFar)
	: IsPerspective(false), Top(top), Bottom(bottom), Near(zNear), Far(zFar)
{
	ASSERTE(false, "Orthographics projection is not yet implemented");
}
