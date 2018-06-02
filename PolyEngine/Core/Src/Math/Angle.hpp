#pragma once

#include "Defines.hpp"
#include "Math/BasicMath.hpp"

namespace Poly {

	/// <summary>
	/// Wrapper class for angle expressed in any unit of measurement.
	/// <para>This class provides abstraction for angle measurement units such as degrees and radians.</para>
	/// <para>All functions that require angle as an input should be using this class in order to ensure maximum compatibility.</para>
	/// <para>It has very little overhead and most of the implementation is constexpr. Angle value is stored internally in radians.</para>
	/// </summary>
	class CORE_DLLEXPORT Angle final : public BaseObjectLiteralType<>
	{
	public:
		/// <summary>Explicit static method for creating Angle class instance with initial value provided in degrees.</summary>
		/// <remarks>Warning, when creating instance from degrees there is possible loss of accuracy due to underlying conversion to radian representation.
		/// This method can be also called via usage of literal "_deg", i.e. "90_deg" will create Angle class instance with 90 degrees angle value.</remarks>
		/// <param name ="deg">Value in degrees.</param>
		/// <returns>Instance of Angle class.</returns>
		static constexpr Angle FromDegrees(float deg) { return Angle(deg * kDeg2Rad); }

		/// <summary>Explicit static method for creating Angle class instance with initial value provided in radians.</summary>
		/// <remarks>This method can be also called via usage of literal "_rad", i.e. "3.1415_rad" will create Angle class instance with 3.1415 radians angle value.</remarks>
		/// <param name ="rad">Value in radians.</param>
		/// <returns>Instance of Angle class.</returns>
		static constexpr Angle FromRadians(float rad) { return Angle(rad); }

		/// <summary>Returns angle value expressed in radians.</summary>
		/// <returns>Angle value in radians.</returns>
		constexpr float AsRadians() const { return Radians; }

		/// <summary>Returns angle value expressed in degrees.</summary>
		/// <returns>Angle value in degrees.</returns>
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
	inline float Sin(Angle angle) { return std::sin(angle.AsRadians()); }
	inline float Cos(Angle angle) { return std::cos(angle.AsRadians()); }
	inline float Tan(Angle angle) { return std::tan(angle.AsRadians()); }
	inline float Ctan(Angle angle) { return 1.0f / std::tan(angle.AsRadians()); }

	inline Angle Asin(float val) { return Angle::FromRadians(std::asin(val)); }
	inline Angle Acos(float val) { return Angle::FromRadians(std::acos(val)); }
	inline Angle Atan(float val) { return Angle::FromRadians(std::atan(val)); }
	inline Angle Atan2(float val1, float val2) { return Angle::FromRadians(std::atan2(val1, val2)); }
	inline Angle Actan(float val) { HEAVY_ASSERTE(!Cmpf(val, 0), "Value cannot be 0."); return Angle::FromRadians(std::atan(1.0f / val)); }
	inline Angle Actan2(float val1, float val2) { return Angle::FromRadians(std::atan2(val2, val1)); }

}
