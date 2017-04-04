#pragma once

#include "Defines.hpp"
#include "BasicMath.hpp"

namespace Poly {

	class DLLEXPORT Color : public BaseObject<>
	{
	public:
		Color();
		Color(float r, float g, float b, float a = 1.0f);

		DLLEXPORT friend std::ostream& operator<< (std::ostream& stream, const Color& color);

		union {
			float Data[4];
			struct
			{
				float R, G, B, A;
			};
		};
	};
}