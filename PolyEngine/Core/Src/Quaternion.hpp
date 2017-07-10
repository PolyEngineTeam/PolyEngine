#pragma once

#include "Defines.hpp"
#include "BasicMath.hpp"
#include "Angle.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"

namespace Poly {

	/// <summary>Represents simple rotation in 3D space.</summary>
	struct CORE_DLLEXPORT EulerAngles { 
		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const EulerAngles& angles);

		Angle X, Y, Z; 
	};

	/// <summary>Used to represent rotation in 3D space without facing the problems of gimbal lock.</summary>
	class ALIGN_16 CORE_DLLEXPORT Quaternion : public BaseObject<>{
	public:
		
		/// <summary>Creates zero-roation quaternion.</summary>
		constexpr Quaternion() : X(0), Y(0), Z(0), W(1) {}

		/// <summary>Creates quaternion with rotation around given axis by provided angle.</summary>
		/// <param name="axis"></param>
		/// <param name="angle"></param>
		Quaternion(const Vector& axis, const Angle& angle);

		/// <summary>Create quaternion from euler angles</summary>
		/// <param name="euler"></param>
		Quaternion(const EulerAngles& euler);

		bool operator==(const Quaternion& rhs) const;
		inline bool operator!=(const Quaternion& rhs) const { return !(*this == rhs); }

		/// <summary>Quaternion composition (multiplication) operator.</summary>
		/// <param name="rhs">Quaternion to multiply with.</param>
		/// <returns>Composed quaternion</returns>
		Quaternion operator*(const Quaternion& rhs) const;

		/// <summary>Quaternion composition (multiplication) and store operator.</summary>
		/// <param name="rhs">Quaternion to multiply with.</param>
		/// <returns>Reference to itself.</returns>
		inline Quaternion& operator*=(const Quaternion& rhs) { *this = *this * rhs; return *this; }

		/// <summary>Vector rotation by quaternion operator.</summary>
		/// <param name="rhs">Vector to rotate.</param>
		/// <returns>Rotated vector.</returns>
		Vector operator*(const Vector& rhs) const;

		/// <summary>Returns length of the quaternion.</summary>
		/// <returns>Length of the quaternion.</returns>
		float Length() const;

		/// <summary>Returns square length of the quaternion.</summary>
		/// <returns>Square length of the quaternion.</returns>
		float LengthSquared() const;

		/// <summary>Conjugates the quaternion (inverses the rotation).</summary>
		/// <returns>Reference to itself.</returns>
		Quaternion& Conjugate();

		/// <summary>Creates conjugated quaternion.</summary>
		/// <returns>Conjugated quaternion.</returns>
		Quaternion GetConjugated() const;

		/// <summary>Normalizes the quaternion.</summary>
		/// <returns>Reference to itself.</returns>
		Quaternion& Normalize();

		/// <summary>Creates normalized quaternion.</summary>
		/// <returns>Normalized quaternion.</returns>
		Quaternion GetNormalized() const;

		/// <summary>Explicit cast to 4x4 rotation matrix</summary>
		explicit operator Matrix() const;

		/// <summary>Sets quaternion values to a rotation around given axis by provided angle.</summary>
		/// <param name="axis"></param>
		/// <param name="angle"></param>
		/// <returns>Reference to itself.</returns>
		Quaternion& SetRotation(const Vector& axis, const Angle& angle);

		/// <summary>Converts quaternion to 4x4 rotation matrix.</summary>
		/// <returns>4x4 rotation matrix</returns>
		EulerAngles ToEulerAngles() const;

		/// <summary>Converts quaternion to euler angles</summary>
		/// <returns>Euler angles decomposition of the quaternion.</returns>
		inline Matrix ToRotationMatrix() const { return static_cast<Matrix>(*this); }

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Quaternion& quat);

		union
		{
		#if !DISABLE_SIMD
			__m128 SimdData;
		#endif
			float Data[4];
			struct { float X,Y,Z,W; };
		};
	};
}
