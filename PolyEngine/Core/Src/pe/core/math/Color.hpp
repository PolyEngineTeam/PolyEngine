#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/BasicMath.hpp>

namespace pe::core::math {
	
	/// <summary>Class representing RGBA color.</summary>
	class CORE_DLLEXPORT Color final : public BaseObjectLiteralType<>
	{
	public:
		static const Color WHITE;
		static const Color BLACK;
		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;

		Color() : R(0), G(0), B(0), A(1)
		{
		}

		constexpr Color(float r, float g, float b, float a = 1.0f) : R(r), G(g), B(b), A(a)
		{
		}

		constexpr Color operator+(const Color& rhs) const {
			return Color(R + rhs.R, G + rhs.G, B + rhs.B, A + rhs.A);
		}

		constexpr Color operator-(const Color& rhs) const {
			return Color(R - rhs.R, G - rhs.G, B - rhs.B, A - rhs.A);
		}

		constexpr Color operator*(const Color& rhs) const {
			return Color(R * rhs.R, G * rhs.G, B * rhs.B, A * rhs.A);
		}

		constexpr Color operator*(const float rhs) const {
			return Color(R * rhs, G * rhs, B * rhs, A * rhs);
		}

		Color& operator+=(const Color& rhs) {
			R += rhs.R; G += rhs.G; B += rhs.B; A += rhs.A;
			return *this;
		}

		Color& operator-=(const Color& rhs) {
			R -= rhs.R; G -= rhs.G; B -= rhs.B; A -= rhs.A;
			return *this;
		}

		bool operator==(const Color& rhs) const { return R == rhs.R && G == rhs.G && B == rhs.B && A == rhs.A; }
		bool operator!=(const Color& rhs) const { return !(*this == rhs); }

		CORE_DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Color& color);

		union {
			std::array<float, 4> Data;
			struct
			{
				float R, G, B, A;
			};
		};
	};
}