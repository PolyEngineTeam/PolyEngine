#pragma once

#include "Defines.hpp"
#include "Math/BasicMath.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "Math/AABox.hpp"

namespace Poly {
	/// <summary>Class representing axis aligned box.</summary>
	class CORE_DLLEXPORT Plane final : public BaseObjectLiteralType<>
	{
	public:
		Plane() : Plane(Vector::ZERO, Vector::UNIT_Z) {}
		Plane(const Vector& point, const Vector& normal)
			: Point(point), Normal(normal)
		{
			HEAVY_ASSERTE(Cmpf(Normal.LengthSquared(), 1.0f), "Versor is not normalized!");
		}

		enum class eObjectLocation
		{
			FRONT,
			INTERSECTS,
			BEHIND,
			_COUNT
		};

		eObjectLocation GetAABoxLocation(const AABox& point) const;
		eObjectLocation GetPointLocation(const Vector& point) const;
		
		const Vector& GetPoint() const { return Point; }
		const Vector& GetNormal() const { return Normal; }

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Plane& color);
	private:
		Vector Point;
		Vector Normal;
	};
}