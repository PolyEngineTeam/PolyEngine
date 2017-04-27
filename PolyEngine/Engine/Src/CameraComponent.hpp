#pragma once

#include "ComponentBase.hpp"
#include "CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT CameraComponent : public ComponentBase
	{
		friend void CameraSystem::CameraUpdatePhase(World*);
	public:
		CameraComponent(float fov, float zNear, float zFar);
		CameraComponent(float top, float bottom, float zNear, float zFar);

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