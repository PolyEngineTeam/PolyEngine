#pragma once

#if defined(_WIN32)
	#include <epoxy/wgl.h>
#elif defined(__linux__)
	#include <epoxy/glx.h>
#else
	#error "Unsupported platform :("
#endif

#include <IRenderingDevice.hpp>

#include "GLShaderProgram.hpp"

#include "RenderingPassBase.hpp"

namespace Poly
{
	class World;

	class DEVICE_DLLEXPORT GLRenderingDevice : public IRenderingDevice
	{
	private:
		enum class eGeometryRenderPassType
		{
			BLINN_PHONG,
			TRANSPARENT_GEOMETRY,
			DEBUG_NORMALS,
			TEXT_2D,
			_COUNT
		};

		enum class ePostprocessRenderPassType
		{
			VINETTE,
			BACKGROUND,
			FOREGROUND,
			BACKGROUND_LIGHT,
			FOREGROUND_LIGHT,
			_COUNT
		};

		struct InputOutputBind
		{
			InputOutputBind(const String& name, RenderingTargetBase* target) : Name(name), Target(target) {}
			String Name;
			RenderingTargetBase* Target;
		};

	public:
#if defined(_WIN32)
		GLRenderingDevice(HWND HWnd, RECT Rect);
#elif defined(__linux__)
		GLRenderingDevice(Display* display, Window window, GLXFBConfig fbConfig, const ScreenSize& size);
#else
#error "Unsupported platform :("
#endif

		~GLRenderingDevice();

		GLRenderingDevice(const GLRenderingDevice&) = delete;
		void operator=(const GLRenderingDevice&) = delete;

		void Resize(const ScreenSize& size) override;
		const ScreenSize& GetScreenSize() const override { return ScreenDim; }

		void RenderWorld(World* world) override;
		void Init() override;

		std::unique_ptr<ITextureDeviceProxy> CreateTexture(size_t width, size_t height, eTextureUsageType usage) override;
		std::unique_ptr<ITextFieldBufferDeviceProxy> CreateTextFieldBuffer() override;
		std::unique_ptr<IMeshDeviceProxy> CreateMesh() override;

	private:
		void InitPrograms();
		void EndFrame();

		template <typename T>
		void RegisterGeometryPass(eGeometryRenderPassType type,
			const std::initializer_list<InputOutputBind>& inputs = {},
			const std::initializer_list<InputOutputBind>& outputs = {});

		void RegisterPostprocessPass(ePostprocessRenderPassType type, const String& fragShaderName,
			const std::initializer_list<InputOutputBind>& inputs = {},
			const std::initializer_list<InputOutputBind>& outputs = {});

		template <typename T, typename... Args>
		T* CreateRenderingTarget(Args&&... args);

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

		Dynarray<std::unique_ptr<RenderingTargetBase>> RenderingTargets;

		EnumArray<std::unique_ptr<RenderingPassBase>, eGeometryRenderPassType> GeometryRenderingPasses;
		EnumArray<std::unique_ptr<RenderingPassBase>, ePostprocessRenderPassType> PostprocessRenderingPasses;

		ScreenSize ScreenDim;
	};

	extern GLRenderingDevice* gRenderingDevice;
}

extern "C"
{
#if defined(_WIN32)
	DEVICE_DLLEXPORT Poly::IRenderingDevice* __stdcall PolyCreateRenderingDevice(HWND hwnd, RECT rect);
#elif defined(__linux__)
	DEVICE_DLLEXPORT Poly::IRenderingDevice* PolyCreateRenderingDevice(Display* display, Window window, GLXFBConfig fbConfig, const Poly::ScreenSize& size);
#else
#error "Unsupported platform :("
#endif
}
