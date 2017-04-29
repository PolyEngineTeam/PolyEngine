#pragma once

#include <pmmintrin.h>

#include "Defines.hpp"
#include "BasicMath.hpp"
#include "Vector.hpp"
#include "Angle.hpp"

namespace Poly {

	class Quaternion;

	struct CORE_DLLEXPORT MatrixSkew : public BaseObject<>
	{
		float XY = 0, XZ = 0, YZ = 0;
	};

	class ALIGN_16 CORE_DLLEXPORT Matrix : public BaseObject<>{
	public:
	  Matrix();
	  Matrix(const float data[16], bool rowOrder = true);

	  Matrix(const Matrix& rhs);
	  Matrix& operator=(const Matrix& rhs);

	  bool operator==(const Matrix& rhs) const;
	  inline bool operator!=(const Matrix& rhs) const { return !(*this == rhs); }

	  Matrix operator*(const Matrix& rhs) const;
	  Matrix& operator*=(const Matrix& rhs);

	  Vector operator*(const Vector& rhs) const;

	  /**
	   *  Calculates the determinant of the matrix.
	   *
	   *  @return Determinant of the matrix.
	   */
	  float Det() const;

	  /**
	   *  Initializes matrix as identity matrix.
	   *
	   *  @return Reference to itself.
	   */
	  Matrix& SetIdentity();

	  /**
	   *  Initializes matrix with rotation about X axis.
	   *
	   *  @param[in] angle Angle in degrees.
	   *  @return Reference to itself.
	   */
	  Matrix& SetRotationX(Angle angle);

	  /**
	   *  Initializes matrix with rotation about Y axis.
	   *
	   *  @param[in] angle Angle in degrees.
	   *  @return Reference to itself.
	   */
	  Matrix& SetRotationY(Angle angle);

	  /**
	   *  Initializes matrix with rotation about Z axis.
	   *
	   *  @param[in] angle Angle in degrees.
	   *  @return Reference to itself.
	   */
	  Matrix& SetRotationZ(Angle angle);

	  /**
	   *  Initializes matrix with translation by the desired vector.
	   *
	   *  @param[in] direction Vector describing the translation.
	   *  @return Reference to itself.
	   */
	  Matrix& SetTranslation(const Vector& direction);

	  /**
	   *  Initializes matrix with proportional scaling.
	   *
	   *  @param[in] scale Scale for all three dimensions.
	   *  @return Reference to itself.
	   */
	  Matrix& SetScale(float scale);

	  /**
	   *  Initializes matrix with scaling.
	   *
	   *  @param[in] scale Scale vector for each of the three dimensions.
	   *  @return Reference to itself.
	   */
	  Matrix& SetScale(const Vector& scale);

	  /**
	  *  Initializes matrix with perspective.
	  *
	  *  @param[in] fov Field of view.
	  *  @param[in] aspect Aspect ratio.
	  *  @param[in] near Near Z plane.
	  *  @param[in] far Far Z plane.
	  *  @return Reference to itself.
	  */
	  Matrix& SetPerspective(float fov, float aspect, float near, float far);

	  /**
	   *  Inverses the matrix.
	   *
	   *  @return Reference to itself after the inversion.
	   */
	  Matrix& Inverse();

	  /**
	   *  Creates inversed matrix from this one.
	   *
	   *  @return New, inversed matrix object.
	   */
	  Matrix GetInversed() const;

	  /**
	   *  Transposes the matrix.
	   *
	   *  @return Reference to itself after the transposition.
	   */
	  Matrix& Transpose();

	  /**
	   *  Creates transposed matrix from this one.
	   *
	   *  @return New, transposed matrix object.
	   */
	  Matrix GetTransposed() const;

	  /**
	   *  Returns internal data pointer for processing in functions
	   *  that need it (for example OpenGL frequently does)
	   *
	   *  @return Internal data pointer to float array of 16
	   */
	  const float* GetDataPtr() const;

	  bool Decompose(Vector& translation, Quaternion& rotation, Vector& scale) const;
	  bool Decompose(Vector& translation, Quaternion& rotation, Vector& scale, MatrixSkew& skew, Vector& perspectivePoint) const;

	  CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Matrix& mat);

	  // This structure allows to access vector elements by index or name.
	  union {
	#if !DISABLE_SIMD
		SILENCE_GCC_WARNING(-Wignored-attributes)
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
