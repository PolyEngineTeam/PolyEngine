#pragma once

#include <pe/Defines.hpp>
#include <pe/api/ecs/ComponentBase.hpp>
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

		CameraComponent(::pe::core::math::Angle fov, float zNear, float zFar);
		CameraComponent(float top, float bottom, float left, float right, float zNear, float zFar);

		const ::pe::core::math::Matrix& GetClipFromView() const { return ClipFromView; }
		const ::pe::core::math::Matrix& GetViewFromWorld() const { return ViewFromWorld; }
		const ::pe::core::math::Matrix& GetClipFromWorld() const { return ClipFromWorld; }

		const float GetClippingPlaneNear() const { return Near; }
		const float GetClippingPlaneFar() const { return Far; }
		const ::pe::core::math::Angle& GetFOV() const { return Fov; }
		const ::pe::core::math::Angle& GetTargetFOV() const { return TargetFov; }
		void SetTargetFOV(const ::pe::core::math::Angle& Value) { TargetFov = Value; }
		void SetFOV(const ::pe::core::math::Angle& Value) { Fov = Value; UpdateProjection(); }
		float GetAspect() const { return Aspect; }
		void SetAspect(float aspect) { Aspect = aspect; UpdateProjection(); }
		bool GetIsForcedRatio() const { return IsForcedRatio; }
		void SetForcedRatio(bool value) { IsForcedRatio = value; }
		eRenderingModeType GetRenderingMode() const { return RenderingMode; }
		void SetRenderingMode(eRenderingModeType value) { RenderingMode = value; }
		::pe::core::math::Frustum GetCameraFrustum() const { return CameraFrustum; };

		void UpdateProjection();

		bool IsVisibleToCamera(const Entity* ent) const;

	private:
		::pe::core::math::Matrix ClipFromView;
		::pe::core::math::Matrix ViewFromWorld;
		::pe::core::math::Matrix ClipFromWorld;

		bool IsPerspective = false;

		// Prerpective
		::pe::core::math::Angle Fov = 0_deg;
		::pe::core::math::Angle TargetFov = 0_deg;

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

		::pe::core::math::Frustum CameraFrustum;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, CameraComponent)
}