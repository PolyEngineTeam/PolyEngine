#pragma once

#include <pe/Defines.hpp>
//#include <pe/rd/common/GLUtils.hpp>
//#include <pe/rd/common/PrimitiveQuad.hpp>
//#include <pe/rd/common/PrimitiveCube.hpp>
//#include <Proxy/GLShaderProgram.hpp>

struct SDL_Window;

namespace pe::core::math
{
	class AARect;
}

namespace pe::rd
{
	class DEVICE_DLLEXPORT GLRenderingDevice : public api::rendering::IRenderingDevice
	{
	public:
		GLRenderingDevice(SDL_Window* window, const pe::core::math::Vector2i& size);
		~GLRenderingDevice();

		bool CreateContext();
		void EndFrame();

		GLRenderingDevice(const GLRenderingDevice&) = delete;
		void operator=(const GLRenderingDevice&) = delete;

		void Init() override {}
		void Resize(const pe::core::math::Vector2i& size) override;
		void RenderWorld(api::ecs::Scene* scene) override {}
		const pe::core::math::Vector2i& GetScreenSize() const override { return ScreenDim; }

		std::unique_ptr<api::rendering::ITextureDeviceProxy> CreateTexture(size_t width, size_t height, size_t channels, api::rendering::eTextureUsageType usage) override;
		std::unique_ptr<api::rendering::ICubemapDeviceProxy> CreateCubemap(size_t width, size_t height) override;
		std::unique_ptr<api::rendering::ITextFieldBufferDeviceProxy> CreateTextFieldBuffer() override;
		std::unique_ptr<api::rendering::IMeshDeviceProxy> CreateMesh() override;
		std::unique_ptr<api::rendering::IParticleDeviceProxy> CreateParticle() override;

	private:
		SDL_Window*	Window = nullptr;
		SDL_GLContext Context;
		pe::core::math::Vector2i ScreenDim;
	};
}

extern "C"
{
	DEVICE_DLLEXPORT ::pe::api::rendering::IRenderingDevice* POLY_STDCALL PolyCreateRenderingDevice(SDL_Window* window, const ::pe::core::math::Vector2i& screenSize);
}