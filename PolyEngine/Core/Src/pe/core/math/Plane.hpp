#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/BasicMath.hpp>
#include <pe/core/math/Vector.hpp>
#include <pe/core/math/Matrix.hpp>
#include <pe/core/math/AABox.hpp>

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