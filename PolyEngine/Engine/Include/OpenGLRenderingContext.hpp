#pragma once

#include <Windows.h>
#include "Renderer.hpp"

namespace Poly {

	struct ENGINE_DLLEXPORT OpenGLRenderingContextParams : public IRenderingContextParams {
		OpenGLRenderingContextParams(HWND hwnd, RECT rect);

		HWND HWnd;
		RECT Rect;
	};

	class OpenGLRenderingContext : public IRenderingContext {
	public:
		bool Init(const IRenderingContextParams* context) override;
		void Deinit() override;

		void EndFrame() override;
	private:
		HDC hDC;
		HWND hWnd;
		HGLRC hRC;
	};

}