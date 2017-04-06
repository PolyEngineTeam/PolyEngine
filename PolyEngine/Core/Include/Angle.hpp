#pragma once

#include "Defines.hpp"
#include "BasicMath.hpp"

namespace Poly {

	/// Wrapper class for angle expressed in any unit of measurement.
	///
	/// This class provides abstraction for angle measurement units such as degrees and radians.
	/// All functions that require angle as an input should be using this class in order to ensure maximum compatibility.
	/// It has very little overhead and most of the implementation is constexpr. Angle value is stored in radians.
	class CORE_DLLEXPORT Angle : public BaseObjectLiteralType<>
	{
	public:
		/// Explicit static method for creating Angle class instance with initial value provided in degrees.
		/// Warning, when creating instance from degrees there is possible loss of accuracy due to underlying conversion to radian representation.
		/// This method can be also called via usage of literal "_deg", i.e. "90_deg" will create Angle class instance with 90 degrees angle value.
		/// @param float Value in degrees.
		/// @return Angle Instance of Angle class.
		static constexpr Angle FromDegrees(float deg) { return Angle(deg * kDeg2Rad); }

		/// Explicit static method for creating Angle class instance with initial value provided in radians.
		/// This method can be also called via usage of literal "_rad", i.e. "3.1415_rad" will create Angle class instance with 3.1415 radians angle value.
		/// @param float Value in radians.
		/// @return Angle Instance of Angle class.
		static constexpr Angle FromRadians(float rad) { return Angle(rad); }

		/// Returns angle value expressed in radians.
		/// @return float Angle value in radians.
		constexpr float AsRadians() const { return Radians; }

		/// Returns angle value expressed in degrees.
		/// @return float Angle value in degrees.
		constexpr float AsDegrees() const { return kRad2Deg * Radians; }

		// Negation
		constexpr Angle operator-() const { return Angle(-Radians); }

		// Comparisons
		constexpr bool operator==(const Angle& rhs) const { return Cmpf(Radians, rhs.Radians); }
		constexpr bool operator!=(const Angle& rhs) const { return !(*this == rhs); }
		constexpr bool operator>(const Angle& rhs) const { return Radians > rhs.Radians; }
		constexpr bool operator>=(const Angle& rhs) const { return Radians >= rhs.Radians; }
		constexpr bool operator<(const Angle& rhs) const { return Radians < rhs.Radians; }
		constexpr bool operator<=(const Angle& rhs) const { return Radians <= rhs.Radians; }

		// Sum and subtraction of two angles
		constexpr Angle operator+(const Angle& rhs) const { return Angle(Radians + rhs.Radians); }
		constexpr Angle operator-(const Angle& rhs) const { return Angle(Radians - rhs.Radians); }
		inline Angle& operator+=(const Angle& rhs) { Radians += rhs.Radians; return *this; }
		inline Angle& operator-=(const Angle& rhs) { Radians -= rhs.Radians; return *this; }

		// Multiply and divison of angle by value
		constexpr Angle operator*(float rhs) const { return Angle(Radians * rhs); }
		constexpr Angle operator/(float rhs) const { return Angle(Radians / rhs); }
		inline Angle& operator*=(float rhs) { Radians *= rhs; return *this; }
		inline Angle& operator/=(float rhs) { Radians /= rhs; return *this; }

		// Conversion to stream
		friend std::ostream& operator<< (std::ostream& stream, const Angle& angle) { return stream << angle.AsDegrees() << "_deg"; }
	private:
		constexpr Angle(float radians) : Radians(radians) {}

		static constexpr float kRad2Deg = 57.295779513f;
		static constexpr float kDeg2Rad = 0.01745329252f;
		float Radians;
	};

	// User defined literals for angles
	constexpr Angle operator""_rad(long double value) { return Angle::FromRadians(static_cast<float>(value)); }
	constexpr Angle operator""_rad(unsigned long long int value) { return Angle::FromRadians(static_cast<float>(value)); }
	constexpr Angle operator""_deg(long double value) { return Angle::FromDegrees(static_cast<float>(value)); }
	constexpr Angle operator""_deg(unsigned long long int value) { return Angle::FromDegrees(static_cast<float>(value)); }

	// Trigonometry with Angle
	inline float Sin(Angle angle) { return std::sinf(angle.AsRadians()); }
	inline float Cos(Angle angle) { return std::cosf(angle.AsRadians()); }
	inline float Tan(Angle angle) { return std::tanf(angle.AsRadians()); }
	inline float Ctan(Angle angle) { return 1.0f / std::tanf(angle.AsRadians()); }

	inline Angle Asin(float val) { return Angle::FromRadians(std::asinf(val)); }
	inline Angle Acos(float val) { return Angle::FromRadians(std::acosf(val)); }
	inline Angle Atan(float val) { return Angle::FromRadians(std::atanf(val)); }
	inline Angle Atan2(float val1, float val2) { return Angle::FromRadians(std::atan2f(val1, val2)); }
	inline Angle Actan(float val) { HEAVY_ASSERTE(!Cmpf(val, 0), "Value cannot be 0."); return Angle::FromRadians(std::atanf(1.0f / val)); }
	inline Angle Actan2(float val1, float val2) { return Angle::FromRadians(std::atan2f(val2, val1)); }

}