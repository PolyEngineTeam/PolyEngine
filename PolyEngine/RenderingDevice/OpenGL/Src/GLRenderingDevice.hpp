#pragma once

#include "Common/GLUtils.hpp"
#include <Rendering/IRenderingDevice.hpp>
#include "Common/GLShaderProgram.hpp"
#include <SDL.h>

struct SDL_Window;

namespace Poly
{
	struct PostprocessQuad;
	struct PrimitiveCube;
	struct SceneView;
	class CameraComponent;
	class AARect;
	class World;
	class RenderingPassBase;
	class RenderingTargetBase;
	class IRendererInterface;

	class DEVICE_DLLEXPORT GLRenderingDevice : public IRenderingDevice
	{
		friend class ForwardRenderer;
		friend class TiledForwardRenderer;

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

		enum class eRendererType
		{
			FORWARD,
			TILED_FORWARD,
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

		bool CreateContextHighend();
		bool CreateContextFallback();

		void Init() override;
		void Resize(const ScreenSize& size) override;
		void RenderWorld(World* world) override;
		const ScreenSize& GetScreenSize() const override { return ScreenDim; }

		std::unique_ptr<ITextureDeviceProxy> CreateTexture(size_t width, size_t height, eTextureUsageType usage) override;
		std::unique_ptr<ICubemapDeviceProxy> CreateCubemap(size_t width, size_t height) override;
		std::unique_ptr<ITextFieldBufferDeviceProxy> CreateTextFieldBuffer() override;
		std::unique_ptr<IMeshDeviceProxy> CreateMesh() override;
		std::unique_ptr<IParticleDeviceProxy> CreateParticle() override;

		std::unique_ptr<PostprocessQuad> PrimitivesQuad;
		std::unique_ptr<PrimitiveCube> PrimitivesCube;

		GLuint FallbackWhiteTexture;
		GLuint FallbackBlackTexture;
		GLuint FallbackNormalMap;
		GLuint SSAONoiseMap;

	private:
		void GetExtensions();
		IRendererInterface* CreateRenderer();
		void CreateUtilityTextures();
		void InitPrograms();

		void FillSceneView(SceneView& sceneView);
		void EndFrame();

		void CleanUpResources();

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
		Dynarray<String> OpenGLExtensions;

		eRendererType RendererType;
		IRendererInterface* Renderer;

		EnumArray<std::unique_ptr<RenderingPassBase>, eGeometryRenderPassType> GeometryRenderingPasses;
		EnumArray<std::unique_ptr<RenderingPassBase>, ePostprocessRenderPassType> PostprocessRenderingPasses;
		EnumArray<std::unique_ptr<RenderingTargetBase>, eRenderTargetId> RenderingTargets;
	};

	extern GLRenderingDevice* gRenderingDevice;
}

extern "C"
{
	DEVICE_DLLEXPORT Poly::IRenderingDevice* POLY_STDCALL PolyCreateRenderingDevice(SDL_Window* window, const Poly::ScreenSize& size);
}