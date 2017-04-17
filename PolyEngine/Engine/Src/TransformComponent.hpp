#pragma once

#include "ComponentBase.hpp"

namespace Poly {
	class TransformComponent : public ComponentBase
	{
	public:
		TransformComponent() {};
		TransformComponent(const Vector& direction);
		TransformComponent(float x, float y, float z) : TransformComponent(Vector(x, y, z)) {};
		Vector GetPosition() { return Vector(Matrix.m03, Matrix.m13, Matrix.m23); };
		void SetPosition(const Vector& direction);
		void SetPosition(float x, float y, float z) { SetPosition(Vector(x, y, z)); };
		
	private:
		Matrix Matrix;
	};
}