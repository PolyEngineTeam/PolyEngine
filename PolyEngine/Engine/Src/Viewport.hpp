#pragma once

#include <AABox.hpp>
#include <BaseObject.hpp>

namespace Poly
{
	class CameraComponent;

	class Viewport : public BaseObject<>
	{
	public:
		Viewport(const AABox& rect) : Rect(rect) {}
		void Resize(const AABox& rect) {Rect = rect; }
		const AABox& GetRect() const { return Rect; }
		void SetCamera(CameraComponent* cam) { ASSERTE(cam, "Null camera is not allowed."); Camera = cam; }
		CameraComponent* GetCamera() const { return Camera; }
	private:
		CameraComponent *Camera = nullptr;
		AABox Rect;
	};
}