#include <pe/core/CorePCH.hpp>

#include <pe/core/math/Frustum.hpp>
#include <pe/core/math/Quaternion.hpp>

using namespace pe::core::math;

Frustum::Frustum(Angle fov, float aspect, float zNear, float zFar)
	: FOV(fov)
{
	Update(fov, aspect, zNear, zFar);
}

void Frustum::Update(Angle fov, float aspect, float zNear, float zFar)
{
	ASSERTE(fov > 0_deg, "fov must be greater than zero");
	ASSERTE(zNear > 0, "zNear must be greater than zero");
	ASSERTE(zFar > zNear, "zFar must be greater than zNear");

	FOV = fov;
	Aspect = aspect;
	ZNear = zNear;
	ZFar = zFar;

	const Angle widthFov = fov * aspect;

	// Near Far
	Planes[eFrustumPlane::ZNEAR] = Plane(Vector(0, 0, -zNear), -Vector::UNIT_Z);
	Planes[eFrustumPlane::ZFAR] = Plane(Vector(0, 0, -zFar), Vector::UNIT_Z);

	// Left Right
	const Angle rotY = 90_deg - (widthFov / 2);
	const Vector rightNorm = Quaternion(Vector::UNIT_Y, rotY) * -Vector::UNIT_Z;
	Planes[eFrustumPlane::RIGHT] = Plane(Vector::ZERO, rightNorm);
	const Vector leftNorm = Quaternion(Vector::UNIT_Y, -rotY) * -Vector::UNIT_Z;
	Planes[eFrustumPlane::LEFT] = Plane(Vector::ZERO, leftNorm);

	// Up Down
	const Angle rotX = 90_deg - (fov / 2);
	const Vector upNorm = Quaternion(Vector::UNIT_X, rotX) * -Vector::UNIT_Z;
	Planes[eFrustumPlane::UP] = Plane(Vector::ZERO, upNorm);
	const Vector downNorm = Quaternion(Vector::UNIT_X, -rotX) * -Vector::UNIT_Z;
	Planes[eFrustumPlane::DOWN] = Plane(Vector::ZERO, downNorm);
}

Frustum::eObjectLocation Frustum::GetObjectLocation(const AABox& box, const Matrix& boxToFrustumTransformation) const
{
	AABox transformedBox = box.GetTransformed(boxToFrustumTransformation);
	bool atLeastOneIntersects = false;
	for (eFrustumPlane type : utils::IterateEnum<eFrustumPlane>())
	{
		const Plane& plane = Planes[type];
		Plane::eObjectLocation loc = plane.GetAABoxLocation(transformedBox);

		if (loc == Plane::eObjectLocation::BEHIND)
			return eObjectLocation::OUTSIDE;

		atLeastOneIntersects = atLeastOneIntersects || loc == Plane::eObjectLocation::INTERSECTS;
	}

	return atLeastOneIntersects ? eObjectLocation::PARTIALY_INSIDE : eObjectLocation::INSIDE;
}

//------------------------------------------------------------------------------
namespace pe::core::math {
	std::ostream & operator<<(std::ostream& stream, const Frustum& frust)
	{
		return stream << "Frustum[Fov: " << frust.FOV << " Aspect: " << frust.Aspect 
			<< " zNear: " << frust.ZNear << " zFar: " << frust.ZFar << " ]";
	}

}
