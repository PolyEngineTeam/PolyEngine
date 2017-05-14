#pragma once

#if defined(_WIN32)
	#include <Windows.h>
#elif defined(__linux__)
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <GL/glew.h>
	#include <GL/glxew.h>
	#include <GL/glx.h>
#else
	#error "Unsupported platform :("
#endif

#include "RenderingContext.hpp"

namespace Poly {

#if defined(_WIN32)
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
		bool InitPrograms();

		HDC hDC;
		HWND hWnd;
		HGLRC hRC;
		ScreenSize ScreenDim;
	};
#elif defined(__linux__)
	struct ENGINE_DLLEXPORT OpenGLRenderingContextParams : public IRenderingContextParams {
		OpenGLRenderingContextParams(Display* display, Window window, GLXFBConfig fbConfig) : display(display), window(window), fbConfig(fbConfig) {}
		Display* const display;
		const Window window;
		const GLXFBConfig fbConfig;
	};

	class OpenGLRenderingContext : public IRenderingContext {
	public:
		bool Init(const IRenderingContextParams* ictxParams) override;
		void Deinit() override;

		void EndFrame() override;

		void Resize(const ScreenSize& size) override;
		const ScreenSize& GetScreenSize() const override;
	private:
		bool InitPrograms();

		Display* display;
		Window window;
		GLXContext context;
		ScreenSize ScreenDim;
	};
#else
	#error "Unsupported platform :("
#endif

} //namespace Poly
