#pragma once

#include "Defines.hpp"
#include "BasicMath.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"

namespace Poly {

	struct CORE_DLLEXPORT EulerAngles { Angle X, Y, Z; };

	class ALIGN_16 CORE_DLLEXPORT Quaterion : public BaseObject<>{
	public:
	  constexpr Quaterion() : X(0), Y(0), Z(0), W(1) {}
	  Quaterion(const Vector& axis, const Angle& angle);
	  Quaterion(const EulerAngles& euler);

	  bool operator==(const Quaterion& rhs);
	  inline bool operator!=(const Quaterion& rhs) { return !(*this == rhs); }

	  Quaterion operator*(const Quaterion& rhs) const;
	  inline Quaterion& operator*=(const Quaterion& rhs) { *this = *this * rhs; return *this; }

	  Vector operator*(const Vector& rhs) const;

	  float Length() const;
	  float Length2() const;

	  Quaterion& Conjugate();
	  Quaterion GetConjugated() const;

	  Quaterion& Normalize();
	  Quaterion GetNormalized() const;

	  // cast to matrix
	  explicit operator Matrix() const;

	  EulerAngles ToEulerAngles() const;

	  friend std::ostream& operator<< (std::ostream& stream, const Quaterion& quat);

	  union {
	#if !DISABLE_SIMD
		__m128 SimdData;
	#endif
		float Data[4];
		struct {
		  float X,Y,Z,W;
		};
	  };
	};

}
