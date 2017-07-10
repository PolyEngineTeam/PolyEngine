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

#include <IRenderingDevice.hpp>

#include "GLShaderProgram.hpp"


namespace Poly
{
	class World;

	class DEVICE_DLLEXPORT GLRenderingDevice : public IRenderingDevice
	{
	private:
		enum class eShaderProgramType
		{
			TEST,
			DEBUG_NORMALS,
			TEXT_2D,
			_COUNT
		};

	public:
#if defined(_WIN32)
		GLRenderingDevice(HWND HWnd, RECT Rect);
#elif defined(__linux__)
		GLRenderingDevice(Display* display, Window window, GLXFBConfig fbConfig);
#else
#error "Unsupported platform :("
#endif

		~GLRenderingDevice();

		void Resize(const ScreenSize& size) override;
		const ScreenSize& GetScreenSize() const override { return ScreenDim; }

		void RenderWorld(World* world) override;

		std::unique_ptr<ITextureDeviceProxy> CreateTexture(size_t width, size_t height, eTextureUsageType usage) override;
		std::unique_ptr<ITextFieldBufferDeviceProxy> CreateTextFieldBuffer() override;
		std::unique_ptr<IMeshDeviceProxy> CreateMesh() override;

	private:
		void InitPrograms();
		virtual GLShaderProgram& GetProgram(eShaderProgramType type) { return *ShaderPrograms[type]; }
		void EndFrame();

#if defined(_WIN32)
		HDC hDC;
		HWND hWnd;
		HGLRC hRC;
#elif defined(__linux__)
		Display* display;
		Window window;
		GLXContext context;
#else
#error "Unsupported platform :("
#endif

		ScreenSize ScreenDim;
		EnumArray<GLShaderProgram*, eShaderProgramType> ShaderPrograms;
	};
}

extern "C"
{
#if defined(_WIN32)
	DEVICE_DLLEXPORT Poly::IRenderingDevice* __stdcall PolyCreateRenderingDevice(HWND hwnd, RECT rect);
#elif defined(__linux__)
	DEVICE_DLLEXPORT Poly::IRenderingDevice* PolyCreateRenderingDevice(Display* display, Window window, GLXFBConfig fbConfig);
#else
#error "Unsupported platform :("
#endif
}
