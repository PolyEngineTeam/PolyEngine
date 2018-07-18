#include "EnginePCH.hpp"

#include "Rendering/Camera/CameraComponent.hpp"

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::CameraComponent)

CameraComponent::CameraComponent(Angle fov,  float zNear, float zFar)
	: IsPerspective(true), Fov(fov), Near(zNear), Far(zFar), RenderingMode(eRenderingModeType::LIT)
{
}

CameraComponent::CameraComponent(float top, float bottom, float left, float right, float zNear, float zFar)
	: IsPerspective(false), Top(top), Bottom(bottom), Left(left), Right(right), Near(zNear), Far(zFar), RenderingMode(eRenderingModeType::LIT)
{
	ASSERTE(false, "Orthographics projection is not yet implemented");
}
