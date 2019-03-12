#pragma once

#include <Defines.hpp>
#include <Math/AARect.hpp>
#include <Rendering/Camera/CameraComponent.hpp>

namespace Poly
{
	class CameraComponent;

	class Viewport : public BaseObject<>
	{
	public:
		Viewport(const AARect& rect) : Rect(rect) {}
		void Resize(const AARect& rect) {Rect = rect; }
		AARect& GetRect() { return Rect; }
		void SetCamera(CameraComponent* cam) { ASSERTE(cam, "Null camera is not allowed."); Camera = cam; }
		CameraComponent* GetCamera() const { return Camera; }
	private:
		CameraComponent *Camera = nullptr;
		AARect Rect;
	};
}