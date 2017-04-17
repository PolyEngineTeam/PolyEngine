#include "TransComp.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
TransComp::TransComp(const Vector& direction)
{
	matrix.SetTranslation(direction);
}