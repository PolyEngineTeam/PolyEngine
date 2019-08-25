#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/AARect.hpp>
#include <Rendering/Camera/CameraComponent.hpp>

namespace Poly
{
	class CameraComponent;

	class Viewport : public ::pe::core::BaseObject<>
	{
	public:
		Viewport(const ::pe::core::math::AARect& rect) : Rect(rect) {}
		void Resize(const ::pe::core::math::AARect& rect) {Rect = rect; }
		::pe::core::math::AARect& GetRect() { return Rect; }
		void SetCamera(CameraComponent* cam) { ASSERTE(cam, "Null camera is not allowed."); Camera = cam; }
		CameraComponent* GetCamera() const { return Camera; }
	private:
		CameraComponent *Camera = nullptr;
		::pe::core::math::AARect Rect;
	};
}