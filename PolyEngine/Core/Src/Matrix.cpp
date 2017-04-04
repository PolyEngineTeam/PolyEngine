#include "Matrix.hpp"
#include "BasicMath.hpp"
#include "SimdMath.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
Matrix::Matrix() { SetIdentity(); }

//------------------------------------------------------------------------------
Matrix::Matrix(const float data[16], bool rowOrder) {
  memmove(Data, data, sizeof(float)*16);
  if(!rowOrder) //TODO this is slow, but good enough for now
    Transpose();
}

//------------------------------------------------------------------------------
Matrix::Matrix(const Matrix& rhs) { *this = rhs; }

//------------------------------------------------------------------------------
Matrix& Matrix::operator=(const Matrix& rhs) {
#if DISABLE_SIMD
  for(int i=0; i<16; ++i) { //row
      Data[i] = rhs.Data[i];
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
  for(int i=0; i<4 && result; ++i)
    result = result && Row[i] == rhs.Row[i];
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
  for(int i=0; i<4; ++i) { //row
    for(int j=0; j<4; ++j) { //column
      ret.Data[4*i + j] = Data[4*i]*rhs.Data[j] + Data[4*i + 1]*rhs.Data[4 + j] + Data[4*i +2]*rhs.Data[8 + j] + Data[4*i + 3]*rhs.Data[12 + j] ;
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
  for(int i=0; i<4; ++i) { //row
      ret.Data[i] = Data[4*i]*rhs.Data[0] + Data[4*i + 1]*rhs.Data[1] + Data[4*i +2]*rhs.Data[2] + Data[4*i + 3]*rhs.Data[3];
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
  memset(this, 0, sizeof(Matrix));
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
  memset(this, 0, sizeof(Matrix));
  Data[0] = scale;
  Data[5] = scale;
  Data[10] = scale;
  Data[15] = 1.0f;
  return *this;
}

//------------------------------------------------------------------------------
Matrix& Matrix::SetScale(const Vector& scale) {
  memset(this, 0, sizeof(Matrix));
  Data[0] = scale.X;
  Data[5] = scale.Y;
  Data[10] = scale.Z;
  Data[15] = 1.0f;
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
  for (int i = 0; i < 4; ++i) { //row
    for (int j = i + 1; j < 4; ++j) { //column
      Swap(Data[4*i + j], Data[4*j + i]);
    }
  }
  return *this;
}

//------------------------------------------------------------------------------
Matrix Matrix::GetTransposed() const {
  Matrix ret = *this;
  return ret.Transpose();
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