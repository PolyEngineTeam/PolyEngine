#pragma once

#include "Defines.hpp"
#include "Math/BasicMath.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "Math/Plane.hpp"
#include "Utils/EnumUtils.hpp"

namespace Poly {

	enum class eFrustumPlane
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		ZFAR,
		ZNEAR,

		_COUNT
	};

	/// <summary>Class representing axis aligned box.</summary>
	class CORE_DLLEXPORT Frustum final : public BaseObjectLiteralType<>
	{
	public:
		Frustum(Angle fov, float aspect, float zNear, float zFar);

		enum class eObjectLocation
		{
			OUTSIDE,
			PARTIALY_INSIDE,
			INSIDE,
			_COUNT
		};

		void Update(Angle fov, float aspect, float zNear, float zFar);

		eObjectLocation GetObjectLocation(const AABox& box, const Matrix& boxToFrustumTransformation) const;

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Frustum& color);

		const EnumArray<Plane, eFrustumPlane>& GetPlanes() const { return Planes; }
		const Angle& GetFov() const { return FOV; }
		const float GetAspect() const { return Aspect; }
		const float GetZNear() const { return ZNear; }
		const float GetZFar() const { return ZFar; }
	private:
		EnumArray<Plane, eFrustumPlane> Planes;
		Angle FOV;
		float Aspect;
		float ZNear;
		float ZFar;
	};
}