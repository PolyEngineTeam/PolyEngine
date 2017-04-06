#pragma once

#include <Windows.h>
#include "Renderer.hpp"

namespace Poly {

	struct ENGINE_DLLEXPORT OpenGLRenderingContext : public IRenderingContext {
		OpenGLRenderingContext(HWND hwnd, RECT rect);

		HWND HWnd;
		RECT Rect;
	};

	class OpenGLRenderer : public IRenderer {
	public:
		virtual bool Init(const IRenderingContext* context) override;
		virtual void Deinit() override;
		virtual void Update(float dt) override;

		virtual void DrawPoint(const Vector& pos, const Color& color) override;
		virtual void DrawLine(const Vector& pos1, const Vector& pos2, const Color& color1, const Color& color2) override;
		virtual void DrawTraingle(const Vector& pos1, const Vector& pos2, const Vector& pos3, const Color& color1, const Color& color2, const Color& color3) override;

	private:
		HDC hDC;
		HWND hWnd;
		HGLRC hRC;
	};

}