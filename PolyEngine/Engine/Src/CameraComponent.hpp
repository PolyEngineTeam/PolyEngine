#pragma once

#include "ComponentBase.hpp"
#include "CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT BaseCameraComponent : public ComponentBase
	{
		friend void CameraSystem::CameraUpdatePhase(World*);
	public:
		BaseCameraComponent(float fov, float near, float far);
		BaseCameraComponent(float top, float bottom, float near, float far);

		const Matrix& GetProjectionMatrix() const { return Projection; }
		const Matrix& GetModelViewMatrix() const { return ModelView; }
		const Matrix& GetMVP() const { return MVP; }
	private:
		Matrix Projection;
		Matrix ModelView;
		Matrix MVP;

		bool IsPerspective = false;
		bool NeedsInit = true;
		// Prerpective
		float Fov;

		// Orthographic
		float Top;
		float Bottom;

		//Common
		float Near;
		float Far;
		float Aspect = 1.f;
	};
}