#include "CorePCH.hpp"

using namespace Poly;

const Quaternion Quaternion::IDENTITY = Quaternion();

//------------------------------------------------------------------------------
Quaternion::Quaternion(const Vector& axis, const Angle& angle) {
	SetRotation(axis, angle);
}

//------------------------------------------------------------------------------
Quaternion::Quaternion(const EulerAngles& euler)
{
  float c1 = Cos(euler.Y * 0.5f);
  float s1 = Sin(euler.Y * 0.5f);
  float c2 = Cos(euler.Z * 0.5f);
  float s2 = Sin(euler.Z * 0.5f);
  float c3 = Cos(euler.X * 0.5f);
  float s3 = Sin(euler.X * 0.5f);

  float c1c2 = c1 * c2;
  float s1s2 = s1 * s2;
  W = c1c2 * c3 - s1s2 * s3;
  X = c1c2 * s3 + s1s2 * c3;
  Y = s1 * c2 * c3 + c1 * s2 * s3;
  Z = c1 * s2 * c3 - s1 * c2 * s3;
}

//------------------------------------------------------------------------------
bool Quaternion::operator==(const Quaternion& rhs) const {
#if DISABLE_SIMD
	return Cmpf(X, rhs.X) && Cmpf(Y, rhs.Y) && Cmpf(Z, rhs.Z) && Cmpf(W, rhs.W)
		|| Cmpf(X, -rhs.X) && Cmpf(Y, -rhs.Y) && Cmpf(Z, -rhs.Z) && Cmpf(W, -rhs.W);
#else
	return _mm_movemask_ps(_mm_cmpf_ps(SimdData, rhs.SimdData)) == 0xf
		|| _mm_movemask_ps(_mm_cmpf_ps(SimdData, _mm_sub_ps(_mm_set1_ps(0.0), rhs.SimdData))) == 0xf;
#endif
}

//------------------------------------------------------------------------------
Quaternion Quaternion::operator*(const Quaternion& rhs) const {
  Quaternion ret;
  ret.W = W * rhs.W - X * rhs.X - Y * rhs.Y - Z * rhs.Z;
  ret.X = W * rhs.X + X * rhs.W + Y * rhs.Z - Z * rhs.Y;
  ret.Y = W * rhs.Y - X * rhs.Z + Y * rhs.W + Z * rhs.X;
  ret.Z = W * rhs.Z + X * rhs.Y - Y * rhs.X + Z * rhs.W;
  return ret;
}

//------------------------------------------------------------------------------
Vector Quaternion::operator*(const Vector& rhs) const {
  HEAVY_ASSERTE(Cmpf(LengthSquared(), 1.0f), "Non unit quaterion");
  Vector tmp(X, Y, Z);
  Vector t = tmp.Cross(rhs) * 2;
  return rhs + t * W + tmp.Cross(t);
}

//------------------------------------------------------------------------------
Quaternion Poly::Quaternion::LookAt(const Vector& pos, const Vector& target, const Vector& up)
{
	Vector forward = (target - pos).Normalize();
	float dot = forward.Dot(-Vector::UNIT_Z);

	if (Cmpf(dot, 1.0f))
		return Quaternion::IDENTITY;
	if (Cmpf(dot, -1.0f))
		return Quaternion(up, 180_deg);
	Quaternion q;
	Angle rotAngle = Acos(dot);
	Vector rotAxis = (-Vector::UNIT_Z).Cross(forward).Normalize();
	return Quaternion(rotAxis, rotAngle);
}

//------------------------------------------------------------------------------
Quaternion Poly::Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	return Quaternion(::Lerp(q1.X,q2.X,t), ::Lerp(q1.Y, q2.Y, t), ::Lerp(q1.Z, q2.Z, t), ::Lerp(q1.W, q2.W, t));
}

//------------------------------------------------------------------------------
Quaternion Poly::Quaternion::Slerp(const Quaternion& q1, Quaternion q2, float t)
{
	// Calculate angle between them.
	float cosHalfTheta = q1.W * q2.W + q1.X * q2.X + q1.Y * q2.Y + q1.Z * q2.Z;
	
	// if q1=q2 or q1=-q2 then theta = 0 and we can return q1
	if (Cmpf(cosHalfTheta, 1.f)) {
		return q1;
	}
	// Calculate temporary values.
	Angle halfTheta = Acos(cosHalfTheta);
	float sinHalfTheta = sqrt(1.0f - cosHalfTheta*cosHalfTheta);
	// if theta = 180 degrees then result is not fully defined
	// we could rotate around any axis normal to q1 or q2
	if (Cmpf(sinHalfTheta, 0.f))
	{ // fabs is floating point absolute
		return Quaternion::Lerp(q1, q2, 0.5f);
	}

	float ratioA = Sin(halfTheta * (1.0f - t)) / sinHalfTheta;
	float ratioB = Sin(halfTheta * t) / sinHalfTheta;
	//calculate Quaternion.
	Quaternion q;
	q.W = (q1.W * ratioA + q2.W * ratioB);
	q.X = (q1.X * ratioA + q2.X * ratioB);
	q.Y = (q1.Y * ratioA + q2.Y * ratioB);
	q.Z = (q1.Z * ratioA + q2.Z * ratioB);
	return q;
}

//------------------------------------------------------------------------------
float Quaternion::Length() const { return std::sqrt(LengthSquared()); }

//------------------------------------------------------------------------------
float Quaternion::LengthSquared() const { return W * W + X * X + Y * Y + Z * Z; }

//------------------------------------------------------------------------------
Quaternion& Quaternion::Conjugate() { X = -X; Y = -Y; Z = -Z; return *this; }

//------------------------------------------------------------------------------
Quaternion Quaternion::GetConjugated() const {
  Quaternion ret = *this;
  ret.Conjugate();
  return ret;
}

//------------------------------------------------------------------------------
Quaternion& Quaternion::Normalize() {
  float iLen = 1.0f / Length();
  W *= iLen;
  X *= iLen;
  Y *= iLen;
  Z *= iLen;
  return *this;
}

//------------------------------------------------------------------------------
Quaternion Quaternion::GetNormalized() const {
  Quaternion ret = *this;
  ret.Normalize();
  return ret;
}

//------------------------------------------------------------------------------
Quaternion::operator Matrix() const {
  Matrix ret;

  ret.m00 = 1 - 2 * Y * Y - 2 * Z * Z;  // W*W + X*X - Y*Y - Z*Z;
  ret.m01 = 2 * X * Y - 2 * W * Z;
  ret.m02 = 2 * X * Z + 2 * W * Y;

  ret.m10 = 2 * X * Y + 2 * W * Z;
  ret.m11 = 1 - 2 * X * X - 2 * Z * Z;  // W*W - X*X + Y*Y - Z*Z;
  ret.m12 = 2 * Y * Z - 2 * W * X;

  ret.m20 = 2 * X * Z - 2 * W * Y;
  ret.m21 = 2 * Y * Z + 2 * W * X;
  ret.m22 = 1 - 2 * Y * Y - 2 * X * X;  // W*W - X*X - Y*Y + Z*Z;

  return ret;
}

Quaternion & Poly::Quaternion::SetRotation(const Vector & axis, const Angle & angle)
{
	Angle halfAngle = angle * 0.5f;
	float s = Sin(halfAngle);

	W = Cos(halfAngle);
	X = axis.X * s;
	Y = axis.Y * s;
	Z = axis.Z * s;
	return *this;
}

//------------------------------------------------------------------------------
EulerAngles Quaternion::ToEulerAngles() const {

  float sinus = 2 * (W * Y - X * Z);
  sinus = Clamp(sinus, -1.0f, 1.0f); // need to clamp because of float precission problems

  return { Atan2(2 * (W * X + Y * Z), 1 - 2 * (X * X + Y * Y)),
           Asin(sinus),
           Atan2(2 * (W * Z + X * Y), 1 - 2 * (Y * Y + Z * Z)) };
}

namespace Poly {

	std::ostream& operator<<(std::ostream& stream, const EulerAngles& angles)
	{
		return stream << "Euler[ " << angles.X << " " << angles.Y << " " << angles.Z << " ]";
	}

	//------------------------------------------------------------------------------
	std::ostream& operator<< (std::ostream& stream, const Quaternion& quat) {
		return stream << "Quat[ " << quat.X << " " << quat.Y << " " << quat.Z << " " << quat.W << " ]";

		//EulerAngles euler = quat.ToEulerAngles();
		//return stream << "Quat[ " << euler.X << " " << euler.Y << " " << euler.Z << " ]";
	}
}
