#pragma once

#include <pmmintrin.h>

#include "Defines.hpp"
#include "BasicMath.hpp"
#include "Vector.hpp"
#include "Angle.hpp"

namespace Poly
{
	class Quaternion;

	/// <summary>Class representing skew in 3 dimensions of a matrix transformation.</summary>
	struct CORE_DLLEXPORT MatrixSkew : public BaseObject<>
	{
		float XY = 0, XZ = 0, YZ = 0;
	};

	/// <summary>Class representing 4x4 matrix in row-major order. It takes advantage of SIMD (if possible).</summary>
	class ALIGN_16 CORE_DLLEXPORT Matrix : public BaseObject<>{
	public:
		Matrix();
		Matrix(const float data[16], bool rowOrder = true);
		Matrix(const Matrix& rhs);
		Matrix& operator=(const Matrix& rhs);

		bool operator==(const Matrix& rhs) const;
		inline bool operator!=(const Matrix& rhs) const { return !(*this == rhs); }

		/// <summary>Matrix-Matrix multiplication operator.</summary>
		Matrix operator*(const Matrix& rhs) const;

		/// <summary>Matrix-Matrix multiplication (with store) operator.</summary>
		Matrix& operator*=(const Matrix& rhs);

		/// <summary>Matrix-Vector multiplication operator.</summary>
		Vector operator*(const Vector& rhs) const;

		/// <summary>Calculates the determinant of the matrix.</summary>
		/// <returns>Determinant of the matrix.</returns>
		float Det() const;

		/// <summary>Initializes matrix as identity matrix.</summary>
		/// <returns>Reference to itself.</returns>
		Matrix& SetIdentity();

		/// <summary>Initializes matrix with rotation about X axis.</summary>
		/// <param name="angle"></param>
		/// <returns>Reference to itself.</returns>
		Matrix& SetRotationX(Angle angle);

		/// <summary>Initializes matrix with rotation about Y axis.</summary>
		/// <param name="angle"></param>
		/// <returns>Reference to itself.</returns>
		Matrix& SetRotationY(Angle angle);

		/// <summary>Initializes matrix with rotation about Z axis.</summary>
		/// <param name="angle"></param>
		/// <returns>Reference to itself.</returns>
		Matrix& SetRotationZ(Angle angle);

		/// <summary>Initializes matrix with translation by the desired vector.</summary>
		/// <param name="direction">Vector describing the translation.</param>
		/// <returns>Reference to itself.</returns>
		Matrix& SetTranslation(const Vector& direction);

		/// <summary>Initializes matrix with uniform scaling.</summary>
		/// <param name="scale">Scale for all three dimensions.</param>
		/// <returns>Reference to itself.</returns>
		Matrix& SetScale(float scale);

		/// <summary>Initializes matrix with scaling.</summary>
		/// <param name="scale">Scale vector for each of the three dimensions.</param>
		/// <returns>Reference to itself.</returns>
		Matrix& SetScale(const Vector& scale);

		/// <summary>Initializes matrix with perspective projection.</summary>
		/// <param name="fov">Field of view angle.</param>
		/// <param name="aspect">Aspect ratio.</param>
		/// <param name="near">Near Z plane.</param>
		/// <param name="far">Far Z plane.</param>
		/// <returns>Reference to itself.</returns>
		Matrix& SetPerspective(Angle fov, float aspect, float near, float far);


		/// <summary>Initializes matrix with orthographics projection</summary>
		/// <param name="top">Top dimension.</param>
		/// <param name="bottom">Bottom dimension.</param>
		/// <param name="left">Left dimension.</param>
		/// <param name="right">Right dimension</param>
		/// <param name="near">Near Z plane.</param>
		/// <param name="far">Far Z plane.</param>
		/// <returns>Reference to itself.</returns>
		Matrix& SetOrthographic(float top, float bottom, float left, float right, float near, float far);

		/// <summary>Inverses the matrix.</summary>
		/// <returns>Reference to itself after the inversion.</returns>
		Matrix& Inverse();

		/// <summary>Creates inversed matrix from this one.</summary>
		/// <returns>New, inversed matrix object.</returns>
		Matrix GetInversed() const;

		/// <summary>Transposes the matrix.</summary>
		/// <returns>Reference to itself after the transposition.</returns>
		Matrix& Transpose();

		/// <summary>Creates transposed matrix from this one.</summary>
		/// <returns>New, transposed matrix object.</returns>
		Matrix GetTransposed() const;

		/// <summary>Returns internal data pointer which is organized in row-major order.</summary>
		/// <returns>Internal data pointer to float array of 16 values.</returns>
		const float* GetDataPtr() const;

		/// <summary>Performs decomposition of the matrix to separate translation, rotation and scale.</summary>
		/// <remarks>This method will assert if the decomposed matrix contains non-linear factors (like projection or skew). In such case, use the other overload.</remarks>
		/// <param name="translation">Mutable reference to a vector that will be set to the decomposed transaltion value.</param>
		/// <param name="rotation">Mutable reference to a quaternion that will be set to the decomposed rotation value.</param>
		/// <param name="scale">Mutable reference to a vector that will be set to the decomposed scale value.</param>
		/// <returns>True if decomposition was sucesfull, false otherwise.</returns>
		bool Decompose(Vector& translation, Quaternion& rotation, Vector& scale) const;

		/// <summary>Performs decomposition of the matrix to separate translation, rotation, scale, skew and perspective.</summary>
		/// <param name="translation">Mutable reference to a vector that will be set to the decomposed transaltion value.</param>
		/// <param name="rotation">Mutable reference to a quaternion that will be set to the decomposed rotation value.</param>
		/// <param name="scale">Mutable reference to a vector that will be set to the decomposed scale value.</param>
		/// <param name="skew">Mutable reference to a MatrixSkew that will be set to the decomposed skew value.</param>
		/// <param name="perspectivePoint">Mutable reference to a vector that will be set to the decomposed perspective point value.</param>
		/// <returns>True if decomposition was sucesfull, false otherwise.</returns>
		bool Decompose(Vector& translation, Quaternion& rotation, Vector& scale, MatrixSkew& skew, Vector& perspectivePoint) const;

	  CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Matrix& mat);

	  // This structure allows to access vector elements by index or name.
		union {
		#if !DISABLE_SIMD
			SILENCE_GCC_WARNING(-Wignored-attributes, "__m128 has an alignment attribute that gets ignored, but it doesn't matter since we explicitly set alignment here")
			alignas(16) std::array<__m128, 4> SimdRow;
			UNSILENCE_GCC_WARNING()
		#endif //!DISABLE_SIMD
			alignas(16) std::array<float, 16> Data;
			struct alignas(16) {
				float m00, m01, m02, m03;
				float m10, m11, m12, m13;
				float m20, m21, m22, m23;
				float m30, m31, m32, m33;
			};
		};
	};

}
