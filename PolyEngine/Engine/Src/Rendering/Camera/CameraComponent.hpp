#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Rendering/Camera/CameraSystem.hpp>
#include <Math/Frustum.hpp>

namespace Poly {

	enum class eRenderingModeType
	{
		LIT,
		UNLIT,
		WIREFRAME,
		DEBUG_NORMALS,
		DEBUG_NORMALS_WIREFRAME,
		IMMEDIATE_DEBUG,
		_COUNT
	};

	class ENGINE_DLLEXPORT CameraComponent : public ComponentBase
	{
		friend void CameraSystem::CameraUpdatePhase(Scene*);
	public:
		RTTI_DECLARE_COMPONENT(::Poly::CameraComponent) { NO_RTTI_PROPERTY(); }

		CameraComponent(Angle fov, float zNear, float zFar);
		CameraComponent(float top, float bottom, float left, float right, float zNear, float zFar);

		const Matrix& GetClipFromView() const { return ClipFromView; }
		const Matrix& GetViewFromWorld() const { return ViewFromWorld; }
		const Matrix& GetClipFromWorld() const { return ClipFromWorld; }

		const float GetClippingPlaneNear() const { return Near; }
		const float GetClippingPlaneFar() const { return Far; }
		const Angle& GetFOV() const { return Fov; }
		const Angle& GetTargetFOV() const { return TargetFov; }
		void SetTargetFOV(const Angle& Value) { TargetFov = Value; }
		void SetFOV(const Angle& Value) { Fov = Value; UpdateProjection(); }
		float GetAspect() const { return Aspect; }
		void SetAspect(float aspect) { Aspect = aspect; UpdateProjection(); }
		bool GetForcedRatio() const { return IsForcedRatio; }
		void SetForcedRatio(bool value) { IsForcedRatio = value; }
		eRenderingModeType GetRenderingMode() const { return RenderingMode; }
		void SetRenderingMode(eRenderingModeType value) { RenderingMode = value; }
		Optional<Frustum> GetCameraFrustum() const { return CameraFrustum; };

		void UpdateProjection();

		bool IsVisibleToCamera(const Entity* ent) const;

		// debug
		float BiasMin = 0.01f;
		float BiasMax = 0.05f;
		float PolygonOffset = 0.0f;
		float PolygonUnits = 0.0f;
		float EVSMExponentPositive = 40.0f;
		float EVSMExponentNegative = 10.0f;
		float VSMBias = 0.0f;
		float LightBleedingReduction = 0.0f;
		//debug

	private:
		Matrix ClipFromView;
		Matrix ViewFromWorld;
		Matrix ClipFromWorld;

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
		
		bool IsForcedRatio = false;
		
		// RenderingMode
		eRenderingModeType RenderingMode;

		Optional<Frustum> CameraFrustum;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, CameraComponent)
}