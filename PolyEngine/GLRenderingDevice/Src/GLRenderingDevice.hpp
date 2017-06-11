#pragma once

#if defined(_WIN32)
	#include <Windows.h>
#elif defined(__linux__)
	#error "Unsupported platform :("
#else
	#error "Unsupported platform :("
#endif

#include <IRenderingDevice.hpp>

#include "ShaderProgram.hpp"


namespace Poly
{
	class World;

	enum class eShaderProgramType {
		TEST,
		DEBUG_NORMALS,
		TEXT_2D,
		_COUNT
	};

	class DEVICE_DLLEXPORT GLRenderingDevice : public IRenderingDevice
	{
	public:
#if defined(_WIN32)
		GLRenderingDevice(HWND HWnd, RECT Rect);
#elif defined(__linux__)
#error "Unsupported platform :("
#else
#error "Unsupported platform :("
#endif

		~GLRenderingDevice();

		void Resize(const ScreenSize& size) override;
		const ScreenSize& GetScreenSize() const override { return ScreenDim; }
	
		void RenderWorld(World* world) override;

		std::unique_ptr<ITextureDeviceProxy> CreateTexture(size_t width, size_t height, eTextureUsageType usage) override;
		std::unique_ptr<ITextFieldBufferDeviceProxy> CreateTextFieldBuffer() override;
	private:
		virtual ShaderProgram& GetProgram(eShaderProgramType type) { return *ShaderPrograms[type]; }
		void EndFrame();

#if defined(_WIN32)
		HDC hDC;
		HWND hWnd;
		HGLRC hRC;
#elif defined(__linux__)
#error "Unsupported platform :("
#else
#error "Unsupported platform :("
#endif

		ScreenSize ScreenDim;

		EnumArray<ShaderProgram*, eShaderProgramType> ShaderPrograms;
	};

#if defined(_WIN32)
	DEVICE_DLLEXPORT IRenderingDevice* CreateRenderingDevice(HWND hwnd, RECT rect);
#elif defined(__linux__)
#error "Unsupported platform :("
#else
#error "Unsupported platform :("
#endif
	
}