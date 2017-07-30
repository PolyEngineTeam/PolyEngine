#pragma once

#include "ComponentBase.hpp"
#include "CameraSystem.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT CameraComponent : public ComponentBase
	{
		friend void CameraSystem::CameraUpdatePhase(World*);
	public:
		CameraComponent(Angle fov, float zNear, float zFar);
		CameraComponent(float top, float bottom, float left, float right, float zNear, float zFar);

		const Matrix& GetProjectionMatrix() const { return Projection; }
		const Matrix& GetModelViewMatrix() const { return ModelView; }
		const Matrix& GetMVP() const { return MVP; }

		const Angle& GetFOV() const { return Fov; }
		const Angle& GetTargetFOV() const { return TargetFov; }
		void SetTargetFOV(Angle& Value) { TargetFov = Value; }
		void SetFOV(Angle& Value) { Fov = Value; }
		float GetAspect() const { return Aspect; }
		void SetTargetTranslation(Vector Value) { TargetTranslation = Value; }
		const Vector& GetTargetTranslation() const { return TargetTranslation; }

	private:
		Matrix Projection;
		Matrix ModelView;
		Matrix MVP;

		Vector TargetTranslation;

		bool IsPerspective = false;
		// Prerpective
		Angle Fov = 0_deg;
		Angle TargetFov = 0_deg;

		// Orthographic
		float Top = 0.f;
		float Bottom = 0.f;
		float Left = 0.f;
		float Right = 0.f;

		//Common
		float Near = 0.f;
		float Far = 0.f;
		float Aspect = 1.f;
	};
}