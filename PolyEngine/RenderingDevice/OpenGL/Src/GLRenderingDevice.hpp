#pragma once

#include "Common/GLUtils.hpp"
#include <Rendering/IRenderingDevice.hpp>
#include "Common/GLShaderProgram.hpp"
#include <SDL.h>

struct SDL_Window;

namespace Poly
{
	class CameraComponent;
	class AARect;
	class Scene;
	struct PostprocessQuad;
	struct PrimitiveCube;
	class RenderingPassBase;
	class RenderingTargetBase;
	class RenderingTargetBase;

	class DEVICE_DLLEXPORT GLRenderingDevice : public IRenderingDevice
	{
	private:
		enum class eGeometryRenderPassType
		{
			UNLIT,
			BLINN_PHONG,
			TRANSPARENT_GEOMETRY,
			TRANSPARENT_SPRITESHEET,
			PARTICLES,
			DEBUG_NORMALS,
			DEBUG_NORMALS_WIREFRAME,
			IMMEDIATE_DEBUG,
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

		enum class eRenderTargetId
		{
			COLOR,
			DEPTH,
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

		void RenderWorld(Scene* world) override;
		void Init() override;

		std::unique_ptr<ITextureDeviceProxy> CreateTexture(size_t width, size_t height, eTextureUsageType usage) override;
		std::unique_ptr<ICubemapDeviceProxy> CreateCubemap(size_t width, size_t height) override;
		std::unique_ptr<ITextFieldBufferDeviceProxy> CreateTextFieldBuffer() override;
		std::unique_ptr<IMeshDeviceProxy> CreateMesh() override;
		std::unique_ptr<IParticleDeviceProxy> CreateParticle() override;

	private:
		void InitPrograms();
		void EndFrame();

		void CleanUpResources();

		void RenderLit(Scene* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void RenderUnlit(Scene* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void RenderWireframe(Scene* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void RenderNormals(Scene* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void RenderNormalsWireframe(Scene* world, const AARect& rect, CameraComponent* cameraCmp) const;
		void RenderDebug(Scene* world, const AARect& rect, CameraComponent* cameraCmp) const;

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
		T* CreateRenderingTarget(eRenderTargetId type, Args&&... args);

		SDL_Window* Window;
		SDL_GLContext Context;
		ScreenSize ScreenDim;

		EnumArray<std::unique_ptr<RenderingPassBase>, eGeometryRenderPassType> GeometryRenderingPasses;
		EnumArray<std::unique_ptr<RenderingPassBase>, ePostprocessRenderPassType> PostprocessRenderingPasses;
		EnumArray<std::unique_ptr<RenderingTargetBase>, eRenderTargetId> RenderingTargets;
		
		std::unique_ptr<PostprocessQuad> PostprocessRenderingQuad;
		std::unique_ptr<PrimitiveCube> PrimitiveRenderingCube;
	};

	extern GLRenderingDevice* gRenderingDevice;
}

extern "C"
{
	DEVICE_DLLEXPORT Poly::IRenderingDevice* POLY_STDCALL PolyCreateRenderingDevice(SDL_Window* window, const Poly::ScreenSize& size);
}