#include "CorePCH.hpp"

#include "Color.hpp"

using namespace Poly;

const Color Color::WHITE = Color(1.0f, 1.0f, 1.0f);
const Color Color::BLACK = Color(0.0f, 0.0f, 0.0f);
const Color Color::RED = Color(1.0f, 0.0f, 0.0f);
const Color Color::GREEN = Color(0.0f, 1.0f, 0.0f);
const Color Color::BLUE = Color(0.0f, 0.0f, 1.0f);

std::ostream& operator<< (std::ostream& stream, const Color& color)
{
	return stream << "Color[ " << color.R << " " << color.G << " " << color.B << " " << color.A << " ]";
}