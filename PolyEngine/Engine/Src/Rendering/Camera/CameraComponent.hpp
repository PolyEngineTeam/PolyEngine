#pragma once

#include <pe/Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Rendering/Camera/CameraSystem.hpp>
#include <pe/core/math/Frustum.hpp>

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

		CameraComponent(core::math::Angle fov, float zNear, float zFar);
		CameraComponent(float top, float bottom, float left, float right, float zNear, float zFar);

		const core::math::Matrix& GetClipFromView() const { return ClipFromView; }
		const core::math::Matrix& GetViewFromWorld() const { return ViewFromWorld; }
		const core::math::Matrix& GetClipFromWorld() const { return ClipFromWorld; }

		const float GetClippingPlaneNear() const { return Near; }
		const float GetClippingPlaneFar() const { return Far; }
		const core::math::Angle& GetFOV() const { return Fov; }
		const core::math::Angle& GetTargetFOV() const { return TargetFov; }
		void SetTargetFOV(const core::math::Angle& Value) { TargetFov = Value; }
		void SetFOV(const core::math::Angle& Value) { Fov = Value; UpdateProjection(); }
		float GetAspect() const { return Aspect; }
		void SetAspect(float aspect) { Aspect = aspect; UpdateProjection(); }
		bool GetIsForcedRatio() const { return IsForcedRatio; }
		void SetForcedRatio(bool value) { IsForcedRatio = value; }
		eRenderingModeType GetRenderingMode() const { return RenderingMode; }
		void SetRenderingMode(eRenderingModeType value) { RenderingMode = value; }
		core::math::Frustum GetCameraFrustum() const { return CameraFrustum; };

		void UpdateProjection();

		bool IsVisibleToCamera(const Entity* ent) const;

	private:
		core::math::Matrix ClipFromView;
		core::math::Matrix ViewFromWorld;
		core::math::Matrix ClipFromWorld;

		bool IsPerspective = false;

		// Prerpective
		core::math::Angle Fov = 0_deg;
		core::math::Angle TargetFov = 0_deg;

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

		core::math::Frustum CameraFrustum;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, CameraComponent)
}