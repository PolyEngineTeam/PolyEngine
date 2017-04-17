#include "TransComp.hpp"

using namespace Poly;

//------------------------------------------------------------------------------
TransComp::TransComp(const Vector& direction)
{
	setPosition(direction);
}

//------------------------------------------------------------------------------
void TransComp::setPosition(const Vector& direction)
{
	matrix.SetTranslation(direction);
}