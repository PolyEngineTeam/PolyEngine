#include "CorePCH.hpp"

#include "Vector.hpp"

using namespace Poly;

const Vector Vector::ZERO = Vector(0, 0, 0);
const Vector Vector::UNIT_X = Vector(1, 0, 0);
const Vector Vector::UNIT_Y = Vector(0, 1, 0);
const Vector Vector::UNIT_Z = Vector(0, 0, 1);

//------------------------------------------------------------------------------
bool Vector::operator==(const Vector& rhs) const {
#if DISABLE_SIMD
  return Cmpf(X, rhs.X) && Cmpf(Y, rhs.Y) && Cmpf(Z, rhs.Z) && Cmpf(W, rhs.W);
#else
  return _mm_movemask_ps(_mm_cmpf_ps(SimdData, rhs.SimdData)) == 0xf;
#endif
}

//------------------------------------------------------------------------------
Vector Vector::operator+(const Vector& rhs) const {
#if DISABLE_SIMD
  return Vector(X+rhs.X, Y+rhs.Y, Z+rhs.Z);
#else
  return Vector(_mm_add_ps(SimdData, rhs.SimdData));
#endif
}

//------------------------------------------------------------------------------
Vector Vector::operator-(const Vector& rhs) const {
#if DISABLE_SIMD
  return Vector(X-rhs.X, Y-rhs.Y, Z-rhs.Z);
#else
  return Vector(_mm_sub_ps(SimdData, rhs.SimdData));
#endif
}

//------------------------------------------------------------------------------
Vector& Vector::operator+=(const Vector& rhs) {
#if DISABLE_SIMD
  X+=rhs.X; Y+=rhs.Y; Z+=rhs.Z;
#else
  SimdData = _mm_add_ps(SimdData, rhs.SimdData);
  W = 1.0f;
#endif
  return *this;
}

//------------------------------------------------------------------------------
Vector& Vector::operator-=(const Vector& rhs){
#if DISABLE_SIMD
  X-=rhs.X; Y-=rhs.Y; Z-=rhs.Z;
#else
  SimdData = _mm_sub_ps(SimdData, rhs.SimdData);
  W = 1.0f;
#endif
  return *this;
}

//------------------------------------------------------------------------------
Vector Vector::operator*(float rhs) const {
#if DISABLE_SIMD
  return Vector(X*rhs, Y*rhs, Z*rhs);
#else
  __m128 v = _mm_set_ps1(rhs);
  return Vector(_mm_mul_ps(SimdData, v));
#endif
}

//------------------------------------------------------------------------------
Vector Vector::operator/(float rhs) const {
#if DISABLE_SIMD
  return Vector(X/rhs, Y/rhs, Z/rhs);
#else
  __m128 v = _mm_set_ps1(rhs);
  return Vector(_mm_div_ps(SimdData, v));
#endif
}

//------------------------------------------------------------------------------
Vector& Vector::operator*=(float rhs) {
#if DISABLE_SIMD
  X*=rhs; Y*=rhs; Z*=rhs;
#else
  __m128 v = _mm_set_ps1(rhs);
  SimdData = _mm_mul_ps(SimdData, v);
  W = 1.0f;
#endif
  return *this;
}

//------------------------------------------------------------------------------
Vector& Vector::operator/=(float rhs) {
#if DISABLE_SIMD
  X/=rhs; Y/=rhs; Z/=rhs;
#else
  __m128 v = _mm_set_ps1(rhs);
  SimdData = _mm_div_ps(SimdData, v);
  W = 1.0f;
#endif
  return *this;
}

//------------------------------------------------------------------------------
//TODO maybe inline this?
float Vector::Length() const { return std::sqrt(Length2()); }
float Vector::Length2() const { return Dot(*this); }

//------------------------------------------------------------------------------
float Vector::Dot(const Vector& rhs) const {
#if DISABLE_SIMD
  return X*rhs.X + Y*rhs.Y + Z*rhs.Z;
#else
  float ret;
  _mm_store_ss(&ret, _mm_dot_ps(SimdData, rhs.SimdData));
  return ret;
#endif
}

//------------------------------------------------------------------------------
Vector Vector::Cross(const Vector& rhs) const {
  //TODO vectorize
#if DISABLE_SIMD
  Vector ret;
  ret.X = Y * rhs.Z - Z * rhs.Y;
  ret.Y = Z * rhs.X - X * rhs.Z;
  ret.Z = X * rhs.Y - Y * rhs.X;
  return ret;
#else
// Shuffle must be an immediate value on some compilers.
#define YZXMask _MM_SHUFFLE(0, 0, 2, 1)
#define ZXYMask _MM_SHUFFLE(0, 1, 0, 2)
  // first subtraction part
  __m128 tmp_yzx = _mm_shuffle_ps(SimdData, SimdData, YZXMask);
  __m128 tmp_zxy = _mm_shuffle_ps(rhs.SimdData, rhs.SimdData, ZXYMask);
  __m128 mult1 = _mm_mul_ps(tmp_yzx, tmp_zxy);
  // second subtraction part
  tmp_zxy = _mm_shuffle_ps(SimdData, SimdData, ZXYMask);
  tmp_yzx = _mm_shuffle_ps(rhs.SimdData, rhs.SimdData, YZXMask);
  __m128 mult2 = _mm_mul_ps(tmp_zxy, tmp_yzx);

  return Vector(_mm_sub_ps(mult1, mult2));
// Cleanup the defines
#undef YZXMask
#undef ZXYMask

#endif
}

//------------------------------------------------------------------------------
Vector& Vector::Normalize() {
#if DISABLE_SIMD
  *this/=Length();
#else
  __m128 dot = _mm_dot_ps(SimdData, SimdData);
  SimdData = _mm_div_ps(SimdData, _mm_sqrt_ps(dot));
  W = 1;
#endif
  return *this;
}

//------------------------------------------------------------------------------
Vector Vector::GetNormalized() const {
#if DISABLE_SIMD
  return *this/Length();
#else
  __m128 dot = _mm_dot_ps(SimdData, SimdData);
  return Vector(_mm_div_ps(SimdData, _mm_sqrt_ps(dot)));
#endif
}

namespace Poly {
	//------------------------------------------------------------------------------
	std::ostream& operator<< (std::ostream& stream, const Vector& vec) {
		return stream << "Vec[ " << vec.X << " " << vec.Y << " " << vec.Z << " ]";
	}
}
