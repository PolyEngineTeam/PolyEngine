#include "CorePCH.hpp"

#include "Matrix.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Matrix::Matrix() { SetIdentity(); }

//------------------------------------------------------------------------------
Matrix::Matrix(const float data[16], bool rowOrder) {
  memmove(Data.data(), data, sizeof(float)*16);
  if(!rowOrder) //TODO this is slow, but good enough for now
    Transpose();
}

//------------------------------------------------------------------------------
Matrix::Matrix(const Matrix& rhs) { *this = rhs; }

//------------------------------------------------------------------------------
Matrix& Matrix::operator=(const Matrix& rhs) {
  if (&rhs == this) return *this;
#if DISABLE_SIMD
  for(int row=0; row<16; ++i) {
      Data[row] = rhs.Data[row];
  }
#else
  for(int i=0; i<4; ++i)
    SimdRow[i] = rhs.SimdRow[i];
#endif
  return *this;
}

//------------------------------------------------------------------------------
bool Matrix::operator==(const Matrix& rhs) const {
#if DISABLE_SIMD
  bool result = true;
  for(int i=0; i<16 && result; ++i)
    result = result && Cmpf(Data[i], rhs.Data[i]);
  return result;
#else
  __m128 result0 = _mm_cmpf_ps(SimdRow[0], rhs.SimdRow[0]);
  __m128 result1 = _mm_cmpf_ps(SimdRow[1], rhs.SimdRow[1]);
  __m128 result2 = _mm_cmpf_ps(SimdRow[2], rhs.SimdRow[2]);
  __m128 result3 = _mm_cmpf_ps(SimdRow[3], rhs.SimdRow[3]);

  __m128 and0 = _mm_and_ps(result0, result1);
  __m128 and1 = _mm_and_ps(result2, result3);
  __m128 and2 = _mm_and_ps(and0, and1);
  return _mm_movemask_ps(and2) == 0xf;
#endif
}

//------------------------------------------------------------------------------
Matrix Matrix::operator*(const Matrix& rhs) const {
  Matrix ret;
#if DISABLE_SIMD
  for(int row=0; row<4; ++row) {
    for(int col=0; col<4; ++col) {
      ret.Data[4*row + col] = Data[4*row]*rhs.Data[col] + Data[4*row + 1]*rhs.Data[4 + col] + Data[4*row +2]*rhs.Data[8 + col] + Data[4*row + 3]*rhs.Data[12 + col] ;
    }
  }
#else
  Matrix t_rhs = rhs.GetTransposed();
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      __m128 c = _mm_mul_ps(SimdRow[i], t_rhs.SimdRow[j]);
      c = _mm_hadd_ps(c, c);
      c = _mm_hadd_ps(c, c);
      _mm_store_ss(&ret.Data[4*i + j], c);
    }
  }
#endif
  return ret;
}

//------------------------------------------------------------------------------
Matrix& Matrix::operator*=(const Matrix& rhs) { return *this = *this * rhs; }

//------------------------------------------------------------------------------
Vector Matrix::operator*(const Vector& rhs) const {
  Vector ret;
#if DISABLE_SIMD
  for(int row=0; row<4; ++row) {
      ret.Data[row] = Data[4*row]*rhs.Data[0] + Data[4*row + 1]*rhs.Data[1] + Data[4*row +2]*rhs.Data[2] + Data[4*row + 3]*rhs.Data[3];
  }
#else
  for (int i = 0; i < 4; ++i) {
      __m128 c = _mm_mul_ps(SimdRow[i], rhs.SimdData);
      c = _mm_hadd_ps(c, c);
      c = _mm_hadd_ps(c, c);
      _mm_store_ss(&ret.Data[i], c);
  }
#endif
  return ret;
}

//------------------------------------------------------------------------------
float Matrix::Det() const {
  //TODO vectorize
  float minor[4] = {0,0,0,0};

  minor[0] = Data[5]  * Data[10] * Data[15] -
  Data[5]  * Data[11] * Data[14] -
  Data[9]  * Data[6]  * Data[15] +
  Data[9]  * Data[7]  * Data[14] +
  Data[13] * Data[6]  * Data[11] -
  Data[13] * Data[7]  * Data[10];

  minor[1] = -Data[4]  * Data[10] * Data[15] +
  Data[4]  * Data[11] * Data[14] +
  Data[8]  * Data[6]  * Data[15] -
  Data[8]  * Data[7]  * Data[14] -
  Data[12] * Data[6]  * Data[11] +
  Data[12] * Data[7]  * Data[10];

  minor[2] = Data[4]  * Data[9] * Data[15] -
  Data[4]  * Data[11] * Data[13] -
  Data[8]  * Data[5] * Data[15] +
  Data[8]  * Data[7] * Data[13] +
  Data[12] * Data[5] * Data[11] -
  Data[12] * Data[7] * Data[9];

  minor[3] = -Data[4]  * Data[9] * Data[14] +
  Data[4]  * Data[10] * Data[13] +
  Data[8]  * Data[5] * Data[14] -
  Data[8]  * Data[6] * Data[13] -
  Data[12] * Data[5] * Data[10] +
  Data[12] * Data[6] * Data[9];

  return Data[0] * minor[0] + Data[1] * minor[1] + Data[2] * minor[2] + Data[3] * minor[3];
}

//------------------------------------------------------------------------------
Matrix& Matrix::SetIdentity() {
  Data.fill(0);
  Data[0] = 1.0f;
  Data[5] = 1.0f;
  Data[10] = 1.0f;
  Data[15] = 1.0f;
  return *this;
}

//------------------------------------------------------------------------------
Matrix& Matrix::SetRotationX(Angle angle) {
  SetIdentity();
  float s = Sin(angle);
  float c = Cos(angle);

  Data[5] = c;
  Data[6] = -s;
  Data[9] = s;
  Data[10] = c;

  return *this;
}

//------------------------------------------------------------------------------
Matrix& Matrix::SetRotationY(Angle angle) {
  SetIdentity();
  float s = Sin(angle);
  float c = Cos(angle);

  Data[0] = c;
  Data[2] = s;
  Data[8] = -s;
  Data[10] = c;

  return *this;
}

//------------------------------------------------------------------------------
Matrix& Matrix::SetRotationZ(Angle angle) {
  SetIdentity();
  float s = Sin(angle);
  float c = Cos(angle);

  Data[0] = c;
  Data[1] = -s;
  Data[4] = s;
  Data[5] = c;

  return *this;
}

//------------------------------------------------------------------------------
Matrix& Matrix::SetTranslation(const Vector& direction) {
  SetIdentity();
  Data[3] = direction.X;
  Data[7] = direction.Y;
  Data[11] = direction.Z;

  return *this;
}

//------------------------------------------------------------------------------
Matrix& Matrix::SetScale(float scale) {
  Data.fill(0);
  Data[0] = scale;
  Data[5] = scale;
  Data[10] = scale;
  Data[15] = 1.0f;
  return *this;
}

//------------------------------------------------------------------------------
Matrix& Matrix::SetScale(const Vector& scale) {
  Data.fill(0);
  Data[0] = scale.X;
  Data[5] = scale.Y;
  Data[10] = scale.Z;
  Data[15] = 1.0f;
  return *this;
}

//------------------------------------------------------------------------------
Matrix& Poly::Matrix::SetPerspective(float fov, float aspect, float near, float far)
{
	float tanHalfFOV = tan(fov / 2);

	Data[0] = 1.0f / (tanHalfFOV * aspect);
	Data[1] = 0;
	Data[2] = 0;
	Data[3] = 0;

	Data[4] = 0;
	Data[5] = 1.0f / tanHalfFOV;
	Data[6] = 0;
	Data[7] = 0;

	Data[8] = 0;
	Data[9] = 0;
	Data[10] = -(far + near) / (far - near);
	Data[11] = -(2.0f * far * near) / (far - near);

	Data[12] = 0;
	Data[13] = 0;
	Data[14] = -1;
	Data[15] = 0;

	return *this;
}

//------------------------------------------------------------------------------
Matrix& Matrix::Inverse() {
  //TODO vectorize
  Matrix cpy = *this;

  Data[0] = cpy.Data[5]  * cpy.Data[10] * cpy.Data[15] -
  cpy.Data[5]  * cpy.Data[11] * cpy.Data[14] -
  cpy.Data[9]  * cpy.Data[6]  * cpy.Data[15] +
  cpy.Data[9]  * cpy.Data[7]  * cpy.Data[14] +
  cpy.Data[13] * cpy.Data[6]  * cpy.Data[11] -
  cpy.Data[13] * cpy.Data[7]  * cpy.Data[10];

  Data[4] = -cpy.Data[4]  * cpy.Data[10] * cpy.Data[15] +
  cpy.Data[4]  * cpy.Data[11] * cpy.Data[14] +
  cpy.Data[8]  * cpy.Data[6]  * cpy.Data[15] -
  cpy.Data[8]  * cpy.Data[7]  * cpy.Data[14] -
  cpy.Data[12] * cpy.Data[6]  * cpy.Data[11] +
  cpy.Data[12] * cpy.Data[7]  * cpy.Data[10];

  Data[8] = cpy.Data[4]  * cpy.Data[9] * cpy.Data[15] -
  cpy.Data[4]  * cpy.Data[11] * cpy.Data[13] -
  cpy.Data[8]  * cpy.Data[5] * cpy.Data[15] +
  cpy.Data[8]  * cpy.Data[7] * cpy.Data[13] +
  cpy.Data[12] * cpy.Data[5] * cpy.Data[11] -
  cpy.Data[12] * cpy.Data[7] * cpy.Data[9];

  Data[12] = -cpy.Data[4]  * cpy.Data[9] * cpy.Data[14] +
  cpy.Data[4]  * cpy.Data[10] * cpy.Data[13] +
  cpy.Data[8]  * cpy.Data[5] * cpy.Data[14] -
  cpy.Data[8]  * cpy.Data[6] * cpy.Data[13] -
  cpy.Data[12] * cpy.Data[5] * cpy.Data[10] +
  cpy.Data[12] * cpy.Data[6] * cpy.Data[9];

  Data[1] = -cpy.Data[1]  * cpy.Data[10] * cpy.Data[15] +
  cpy.Data[1]  * cpy.Data[11] * cpy.Data[14] +
  cpy.Data[9]  * cpy.Data[2] * cpy.Data[15] -
  cpy.Data[9]  * cpy.Data[3] * cpy.Data[14] -
  cpy.Data[13] * cpy.Data[2] * cpy.Data[11] +
  cpy.Data[13] * cpy.Data[3] * cpy.Data[10];

  Data[5] = cpy.Data[0]  * cpy.Data[10] * cpy.Data[15] -
  cpy.Data[0]  * cpy.Data[11] * cpy.Data[14] -
  cpy.Data[8]  * cpy.Data[2] * cpy.Data[15] +
  cpy.Data[8]  * cpy.Data[3] * cpy.Data[14] +
  cpy.Data[12] * cpy.Data[2] * cpy.Data[11] -
  cpy.Data[12] * cpy.Data[3] * cpy.Data[10];

  Data[9] = -cpy.Data[0]  * cpy.Data[9] * cpy.Data[15] +
  cpy.Data[0]  * cpy.Data[11] * cpy.Data[13] +
  cpy.Data[8]  * cpy.Data[1] * cpy.Data[15] -
  cpy.Data[8]  * cpy.Data[3] * cpy.Data[13] -
  cpy.Data[12] * cpy.Data[1] * cpy.Data[11] +
  cpy.Data[12] * cpy.Data[3] * cpy.Data[9];

  Data[13] = cpy.Data[0]  * cpy.Data[9] * cpy.Data[14] -
  cpy.Data[0]  * cpy.Data[10] * cpy.Data[13] -
  cpy.Data[8]  * cpy.Data[1] * cpy.Data[14] +
  cpy.Data[8]  * cpy.Data[2] * cpy.Data[13] +
  cpy.Data[12] * cpy.Data[1] * cpy.Data[10] -
  cpy.Data[12] * cpy.Data[2] * cpy.Data[9];

  Data[2] = cpy.Data[1]  * cpy.Data[6] * cpy.Data[15] -
  cpy.Data[1]  * cpy.Data[7] * cpy.Data[14] -
  cpy.Data[5]  * cpy.Data[2] * cpy.Data[15] +
  cpy.Data[5]  * cpy.Data[3] * cpy.Data[14] +
  cpy.Data[13] * cpy.Data[2] * cpy.Data[7] -
  cpy.Data[13] * cpy.Data[3] * cpy.Data[6];

  Data[6] = -cpy.Data[0]  * cpy.Data[6] * cpy.Data[15] +
  cpy.Data[0]  * cpy.Data[7] * cpy.Data[14] +
  cpy.Data[4]  * cpy.Data[2] * cpy.Data[15] -
  cpy.Data[4]  * cpy.Data[3] * cpy.Data[14] -
  cpy.Data[12] * cpy.Data[2] * cpy.Data[7] +
  cpy.Data[12] * cpy.Data[3] * cpy.Data[6];

  Data[10] = cpy.Data[0]  * cpy.Data[5] * cpy.Data[15] -
  cpy.Data[0]  * cpy.Data[7] * cpy.Data[13] -
  cpy.Data[4]  * cpy.Data[1] * cpy.Data[15] +
  cpy.Data[4]  * cpy.Data[3] * cpy.Data[13] +
  cpy.Data[12] * cpy.Data[1] * cpy.Data[7] -
  cpy.Data[12] * cpy.Data[3] * cpy.Data[5];

  Data[14] = -cpy.Data[0]  * cpy.Data[5] * cpy.Data[14] +
  cpy.Data[0]  * cpy.Data[6] * cpy.Data[13] +
  cpy.Data[4]  * cpy.Data[1] * cpy.Data[14] -
  cpy.Data[4]  * cpy.Data[2] * cpy.Data[13] -
  cpy.Data[12] * cpy.Data[1] * cpy.Data[6] +
  cpy.Data[12] * cpy.Data[2] * cpy.Data[5];

  Data[3] = -cpy.Data[1] * cpy.Data[6] * cpy.Data[11] +
  cpy.Data[1] * cpy.Data[7] * cpy.Data[10] +
  cpy.Data[5] * cpy.Data[2] * cpy.Data[11] -
  cpy.Data[5] * cpy.Data[3] * cpy.Data[10] -
  cpy.Data[9] * cpy.Data[2] * cpy.Data[7] +
  cpy.Data[9] * cpy.Data[3] * cpy.Data[6];

  Data[7] = cpy.Data[0] * cpy.Data[6] * cpy.Data[11] -
  cpy.Data[0] * cpy.Data[7] * cpy.Data[10] -
  cpy.Data[4] * cpy.Data[2] * cpy.Data[11] +
  cpy.Data[4] * cpy.Data[3] * cpy.Data[10] +
  cpy.Data[8] * cpy.Data[2] * cpy.Data[7] -
  cpy.Data[8] * cpy.Data[3] * cpy.Data[6];

  Data[11] = -cpy.Data[0] * cpy.Data[5] * cpy.Data[11] +
  cpy.Data[0] * cpy.Data[7] * cpy.Data[9] +
  cpy.Data[4] * cpy.Data[1] * cpy.Data[11] -
  cpy.Data[4] * cpy.Data[3] * cpy.Data[9] -
  cpy.Data[8] * cpy.Data[1] * cpy.Data[7] +
  cpy.Data[8] * cpy.Data[3] * cpy.Data[5];

  Data[15] = cpy.Data[0] * cpy.Data[5] * cpy.Data[10] -
  cpy.Data[0] * cpy.Data[6] * cpy.Data[9] -
  cpy.Data[4] * cpy.Data[1] * cpy.Data[10] +
  cpy.Data[4] * cpy.Data[2] * cpy.Data[9] +
  cpy.Data[8] * cpy.Data[1] * cpy.Data[6] -
  cpy.Data[8] * cpy.Data[2] * cpy.Data[5];

  float det = cpy.Data[0] * Data[0] + cpy.Data[1] * Data[4] + cpy.Data[2] * Data[8] + cpy.Data[3] * Data[12];

  HEAVY_ASSERTE(det != 0, "Determinant is equal to 0!");

  float idet = 1.0f/det;
  for(int i=0; i<16; ++i)
    Data[i] *= idet;

  return *this;
}

//------------------------------------------------------------------------------
Matrix Matrix::GetInversed() const {
  Matrix ret = *this;
  return ret.Inverse();
}

//------------------------------------------------------------------------------
Matrix& Matrix::Transpose() {
  for (int row = 0; row < 4; ++row) {
    for (int col = row + 1; col < 4; ++col) {
      std::swap(Data[4*row + col], Data[4*col + row]);
    }
  }
  return *this;
}

//------------------------------------------------------------------------------
Matrix Matrix::GetTransposed() const {
  Matrix ret = *this;
  return ret.Transpose();
}

//------------------------------------------------------------------------------
const float* Matrix::GetDataPtr() const { return Data.data(); }

//------------------------------------------------------------------------------
bool Matrix::Decompose(Vector& translation, Quaternion& rotation, Vector& scale) const
{
	MatrixSkew skew;
	Vector perspPoint;
	//TODO optimize this!
	bool result = Decompose(translation, rotation, scale, skew, perspPoint);
	ASSERTE(!result || (Cmpf(skew.XY, 0) && Cmpf(skew.XZ, 0) && Cmpf(skew.YZ, 0)), "Non zero skew, use the other overload of the method!");
	ASSERTE(!result || perspPoint == Vector(0,0,0), "Non zero perspective, use the other overload of the method!");
	return result;
}

//------------------------------------------------------------------------------
bool Poly::Matrix::Decompose(Vector& translation, Quaternion& rotation, Vector& scale, MatrixSkew& skew, Vector& perspectivePoint) const
{
	if (m33 == 0)
	{
		ASSERTE(false, "This matrix cannot be normalized!");
		return false;
	}

	Matrix local(*this);

	//Normalize matrix
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			local.Data[i * 4 + j] /= local.Data[15];

	// perspectiveMatrix is used to solve for perspective, but it also provides
	// an easy way to test for singularity of the upper 3x3 component.
	Matrix perspective = local;
	perspective.m30 = 0;
	perspective.m31 = 0;
	perspective.m32 = 0;
	perspective.m33 = 1;

	if (perspective.Det() == 0)
	{
		ASSERTE(false, "Determinant of perspective matrix is 0!");
		return false;
	}

	// First, isolate perspective.  This is the messiest.
	if (local.m30 != 0 || local.m31 != 0 || local.m32 != 0) {
		// rightHandSide is the right hand side of the equation.
		Vector rightHandSide;
		rightHandSide.X = local.m30;
		rightHandSide.Y = local.m31;
		rightHandSide.Z = local.m32;
		rightHandSide.W = local.m33;

		// Solve the equation by inverting perspectiveMatrix and multiplying
		// rightHandSide by the inverse.  (This is the easiest way, not
		// necessarily the best.)
		Matrix inversePerspectiveMatrix(perspective.GetInversed());
		Matrix transposedInversePerspectiveMatrix(inversePerspectiveMatrix.GetTransposed());

		perspectivePoint = transposedInversePerspectiveMatrix * rightHandSide;

		// Clear the perspective partition
		local.m30 = 0;
		local.m31 = 0;
		local.m32 = 0;
		local.m33 = 1;
	}
	else {
		// No perspective.
		perspectivePoint = Vector::ZERO;
	}

	// Next take care of translation (easy).
	translation.X = local.m03;
	translation.Y = local.m13;
	translation.Z = local.m23;
	translation.W = 1;
	local.m03 = 0;
	local.m13 = 0;
	local.m23 = 0;

	// Vector4 type and functions need to be added to the common set.
	Vector row[3], pdum3;

	// Now get scale and shear.
	for (int i = 0; i < 3; i++) {
		row[i].X = local.Data[i];
		row[i].Y = local.Data[4 + i];
		row[i].Z = local.Data[8 + i];
	}

	// Compute X scale factor and normalize first row.
	scale.X = row[0].Length();
	row[0].Normalize();

	// Compute XY shear factor and make 2nd row orthogonal to 1st.
	//combine: result = (a * ascl) + (b * bscl)
	skew.XY = row[0].Dot(row[1]);
	row[1] += (row[0] * -skew.XY);

	// Now, compute Y scale and normalize 2nd row.
	scale.Y = row[1].Length();
	row[1].Normalize();
	skew.XY /= scale.Y;

	// Compute XZ and YZ shears, orthogonalize 3rd row.
	skew.XZ = row[0].Dot(row[2]);
	row[2] += (row[0] * -skew.XZ);

	skew.YZ = row[1].Dot(row[2]);
	row[2] += (row[1] * -skew.YZ);

	// Next, get Z scale and normalize 3rd row.
	scale.Z = row[2].Length();
	row[2].Normalize();
	skew.XZ /= scale.Z;
	skew.YZ /= scale.Z;

	// At this point, the matrix (in rows[]) is orthonormal.
	// Check for a coordinate system flip.  If the determinant
	// is -1, then negate the matrix and the scaling factors.
	pdum3 = row[1].Cross(row[2]);

	if (row[0].Dot(pdum3) < 0) {
		for (int i = 0; i < 3; i++) {
			scale.X *= -1;
			row[i].X *= -1;
			row[i].Y *= -1;
			row[i].Z *= -1;
		}
	}

	// Now, get the rotations out, as described in the gem.
	float s, t, x, y, z, w;

	t = row[0].X + row[1].Y + row[2].Z;

	if (t > 0.f) {
		s = 0.5f / sqrtf(t + 1.0f);
		w = 0.25f / s;
		x = (row[2].Y - row[1].Z) * s;
		y = (row[0].Z - row[2].X) * s;
		z = (row[1].X - row[0].Y) * s;
	}
	else if (row[0].X > row[1].Y && row[0].X > row[2].Z)
	{
		s = sqrtf(1.0f + row[0].X - row[1].Y - row[2].Z) * 2.0f; // S=4*qx
		x = 0.25f * s;
		y = (row[0].Y + row[1].X) / s;
		z = (row[0].Z + row[2].X) / s;
		w = (row[2].Y - row[1].Z) / s;
	}
	else if (row[1].Y > row[2].Z)
	{
		s = sqrtf(1.0f + row[1].Y - row[0].X - row[2].Z) * 2.0f; // S=4*qy
		x = (row[0].Y + row[1].X) / s;
		y = 0.25f * s;
		z = (row[1].Z + row[2].Y) / s;
		w = (row[0].Z - row[2].X) / s;
	}
	else
	{
		s = sqrtf(1.0f + row[2].Z - row[0].X - row[1].Y) * 2.0f; // S=4*qz
		x = (row[0].Z + row[2].X) / s;
		y = (row[1].Z + row[2].Y) / s;
		z = 0.25f * s;
		w = (row[1].X - row[0].Y) / s;
	}

	//FIXME: Why the rotation have to be reversed? Maybe this is different coordinate result? Investigate this!
	rotation.X = -x;
	rotation.Y = -y;
	rotation.Z = -z;
	rotation.W = w;

	return true;
}

namespace Poly {
	//------------------------------------------------------------------------------
	std::ostream& operator<< (std::ostream& stream, const Matrix& mat) {
		stream << "Mat[ ";
		for (int i = 0; i < 16; ++i)
			stream << mat.Data[i] << " ";
		return stream << "]";
	}
}
