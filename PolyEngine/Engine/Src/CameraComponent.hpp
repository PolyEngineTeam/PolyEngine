#pragma once

#include "ComponentBase.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT BaseCameraComponent : public ComponentBase
	{
	public:
		BaseCameraComponent(float fov, float aspect, float near, float far) {}

		const Matrix& GetProjectionMatrix() const { return Projection;  }
		const Matrix& GetModelViewMatrix() const { return ModelView; }
		const Matrix& GetMVP() const { return MVP; }
	private:
		Matrix Projection;
		Matrix ModelView;
		Matrix MVP;
	};
}