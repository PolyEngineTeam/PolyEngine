#pragma once

#include "Defines.hpp"
#include "BasicMath.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"

namespace Poly {

	struct CORE_DLLEXPORT EulerAngles { Angle X, Y, Z; };

	class ALIGN_16 CORE_DLLEXPORT Quaternion : public BaseObject<>{
	public:
	  constexpr Quaternion() : X(0), Y(0), Z(0), W(1) {}
	  Quaternion(const Vector& axis, const Angle& angle);
	  Quaternion(const EulerAngles& euler);

	  bool operator==(const Quaternion& rhs);
	  inline bool operator!=(const Quaternion& rhs) { return !(*this == rhs); }

	  Quaternion operator*(const Quaternion& rhs) const;
	  inline Quaternion& operator*=(const Quaternion& rhs) { *this = *this * rhs; return *this; }

	  Vector operator*(const Vector& rhs) const;

	  float Length() const;
	  float Length2() const;

	  Quaternion& Conjugate();
	  Quaternion GetConjugated() const;

	  Quaternion& Normalize();
	  Quaternion GetNormalized() const;

	  // cast to matrix
	  explicit operator Matrix() const;

	  Quaternion& SetRotation(const Vector& axis, const Angle& angle);

	  EulerAngles ToEulerAngles() const;
	  inline Matrix ToRotationMatrix() const { return static_cast<Matrix>(*this); }

	  CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Quaternion& quat);

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
