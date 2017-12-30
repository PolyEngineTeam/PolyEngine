#pragma once

#include "GLUtils.hpp"
#include <IRenderingDevice.hpp>
#include "GLShaderProgram.hpp"
#include <SDL.h>

struct SDL_Window;

namespace Poly
{
	class CameraComponent;
	class AARect;
	class World;
	struct PostprocessQuad;
	struct PrimitiveCube;
	class RenderingPassBase;
	class RenderingTargetBase;

	class DEVICE_DLLEXPORT GLRenderingDevice : public IRenderingDevice
	{
	private:
		enum class eGeometryRenderPassType
		{
			UNLIT,
			BLINN_PHONG,
			TRANSPARENT_GEOMETRY,
			DEBUG_NORMALS,
			DEBUG_NORMALS_WIREFRAME,
			DEBUG,
			TEXT_2D,
			SKYBOX,
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
		GLRenderingDevice(SDL_Window* window, const Poly::ScreenSize& size);
		~GLRenderingDevice();

		GLRenderingDevice(const GLRenderingDevice&) = delete;
		void operator=(const GLRenderingDevice&) = delete;

		void Resize(const ScreenSize& size) override;
		const ScreenSize& GetScreenSize() const override { return ScreenDim; }

		void RenderWorld(World* world) override;
		void Init() override;

		std::unique_ptr<ITextureDeviceProxy> CreateTexture(size_t width, size_t height, eTextureUsageType usage) override;
		std::unique_ptr<ICubemapDeviceProxy> CreateCubemap(size_t width, size_t height) override;
		std::unique_ptr<ITextFieldBufferDeviceProxy> CreateTextFieldBuffer() override;
		std::unique_ptr<IMeshDeviceProxy> CreateMesh() override;

	private:
		void InitPrograms();
		void EndFrame();

		void CleanUpResources();

		void RenderLit(World* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void RenderUnlit(World* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void RenderWireframe(World* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void RenderNormals(World* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void RenderNormalsWireframe(World* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void RenderDebug(World* world, const AARect& rect, CameraComponent* cameraCmp) const;

		template<typename T>
		void RegisterGeometryPass(eGeometryRenderPassType type,
			const std::initializer_list<InputOutputBind>& inputs = {},
			const std::initializer_list<InputOutputBind>& outputs = {});

		template<typename T, class... Args_t>
		void RegisterGeometryPassWithArgs(eGeometryRenderPassType type,
			const std::initializer_list<InputOutputBind>& inputs,
			const std::initializer_list<InputOutputBind>& outputs, Args_t&&... args);

		void RegisterPostprocessPass(ePostprocessRenderPassType type, const String& fragShaderName,
			const std::initializer_list<InputOutputBind>& inputs = {},
			const std::initializer_list<InputOutputBind>& outputs = {});

		template <typename T, typename... Args>
		T* CreateRenderingTarget(Args&&... args);

		SDL_Window* Window;
		SDL_GLContext Context;
		ScreenSize ScreenDim;

		Dynarray<std::unique_ptr<RenderingTargetBase>> RenderingTargets;

		EnumArray<std::unique_ptr<RenderingPassBase>, eGeometryRenderPassType> GeometryRenderingPasses;
		EnumArray<std::unique_ptr<RenderingPassBase>, ePostprocessRenderPassType> PostprocessRenderingPasses;

		std::unique_ptr<PostprocessQuad> PostprocessRenderingQuad;
		std::unique_ptr<PrimitiveCube> PrimitiveRenderingCube;
	};

	extern GLRenderingDevice* gRenderingDevice;
}

extern "C"
{
	DEVICE_DLLEXPORT Poly::IRenderingDevice* POLY_STDCALL PolyCreateRenderingDevice(SDL_Window* window, const Poly::ScreenSize& size);
}