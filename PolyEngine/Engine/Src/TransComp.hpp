#pragma once

#include "ComponentBase.hpp"

namespace Poly {
	class TransComp : public ComponentBase
	{
	public:
		TransComp() {};
		TransComp(const Vector& direction);
		TransComp(float x, float y, float z) : TransComp(Vector(x, y, z)) {};
		Matrix matrix;
		
	private:
	};
}