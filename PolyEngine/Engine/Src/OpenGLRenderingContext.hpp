#pragma once

#include <Windows.h>
#include "RenderingContext.hpp"

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

		void Resize(const ScreenSize& size) override;
		const ScreenSize& GetScreenSize() const override;
	private:
		HDC hDC;
		HWND hWnd;
		HGLRC hRC;
		ScreenSize ScreenDim;
	};

}