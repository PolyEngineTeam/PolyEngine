#pragma once

#include <Rendering/IRenderingDevice.hpp>
#include "Common/GLUtils.hpp"

namespace Poly
{
	struct ScreenSize;

	enum class eRenderTargetType
	{
		NONE,
		COLOR_ATTACHEMENT,
		DEPTH_ATTACHEMENT,
		_COUNT
	};

	class GLTextureDeviceProxy : public ITextureDeviceProxy
	{
	public:
		GLTextureDeviceProxy(size_t width, size_t height, eRenderTargetType internalUsage, GLuint internalFormat);
		GLTextureDeviceProxy(size_t width, size_t height, size_t channels, eTextureUsageType usage);
		virtual ~GLTextureDeviceProxy();
		
		void SetContent(const unsigned char* data) override;
		void SetContentHDR(const float* data) override;
		void SetSubContent(size_t width, size_t height, size_t offsetX, size_t offsetY, const unsigned char* data) override;
		unsigned int GetResourceID() const override { return TextureID; };

		GLuint GetTextureID() const { return TextureID; }

		void Resize(const ScreenSize& size);

	private:
		void InitTextureParams();
		void InitTextureHDR();
		void InitTextureFont();
		void InitTextureGamma();
		void InitTextureLinear();
		void InitTextureRenderTarget();
		void InitTextureNormal();

		size_t Width = 0;
		size_t Height = 0;
		size_t Channels = 0;
		GLuint TextureID = 0;
		GLuint InternalFormat = 0;
		GLuint Format = 0;
		eRenderTargetType RenderTargetType = eRenderTargetType::NONE;
		eTextureUsageType Usage = eTextureUsageType::_COUNT;
	};
}