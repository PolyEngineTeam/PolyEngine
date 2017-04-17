#pragma once

#include "ComponentBase.hpp"

namespace Poly {
	class TransComp : public ComponentBase
	{
	public:
		TransComp() {};
		TransComp(const Vector& direction);
		TransComp(float x, float y, float z) : TransComp(Vector(x, y, z)) {};
		Vector getPosition() { return Vector(matrix.m03, matrix.m13, matrix.m23); };
		void setPosition(const Vector& direction);
		void setPosition(float x, float y, float z) { setPosition(Vector(x, y, z)); };
		
	private:
		Matrix matrix;
	};
}