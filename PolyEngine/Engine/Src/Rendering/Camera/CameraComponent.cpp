#include <EnginePCH.hpp>

#include <Rendering/Camera/CameraComponent.hpp>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::CameraComponent)

CameraComponent::CameraComponent(Angle fov,  float zNear, float zFar)
	: IsPerspective(true), Fov(fov), Near(zNear), Far(zFar),
	RenderingMode(eRenderingModeType::LIT), CameraFrustum(fov, 1.f, zNear, zFar )
{
}

CameraComponent::CameraComponent(float top, float bottom, float left, float right, float zNear, float zFar)
	: IsPerspective(false), Top(top), Bottom(bottom), Left(left), Right(right), Near(zNear), Far(zFar),
	RenderingMode(eRenderingModeType::LIT), CameraFrustum(45_deg, 1.f, zNear, zFar )
{
	ASSERTE(false, "Orthographic projection is not yet implemented");
}

void Poly::CameraComponent::UpdateProjection()
{
	if (IsPerspective)
	{
		ClipFromView.SetPerspective(Fov, Aspect, Near, Far);
		CameraFrustum.Update(Fov, Aspect, Near, Far);
	}
	else
	{
		ClipFromView.SetOrthographic(Top, Bottom, Left, Right, Near, Far);
	}
}

bool Poly::CameraComponent::IsVisibleToCamera(const Entity* ent) const
{
	if (IsPerspective)
	{
		const Frustum::eObjectLocation loc = CameraFrustum.GetObjectLocation(
			ent->GetGlobalBoundingBox(eEntityBoundingChannel::RENDERING), GetViewFromWorld());
		return loc != Frustum::eObjectLocation::OUTSIDE;
	}
	else
	{
		const AABox box({ Left , Bottom, Near }, { Right , Top, Far });
		return box.GetTransformed(GetTransform().GetWorldFromModel())
			.Intersects(ent->GetGlobalBoundingBox(eEntityBoundingChannel::RENDERING));
	}
}
