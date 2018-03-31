#pragma once

#include <Rendering/IRenderingDevice.hpp>
#include "Common/GLUtils.hpp"

namespace Poly
{
	class GLCubemapDeviceProxy : public ICubemapDeviceProxy
	{
	public:
		GLCubemapDeviceProxy(size_t width, size_t height);
		virtual ~GLCubemapDeviceProxy();
		
		void SetContent(const eCubemapSide side, const unsigned char* data) override;

		GLuint GetTextureID() const { return TextureID; }

	private:
		static GLenum GetEnumFromCubemapSide(eCubemapSide type);

		void InitCubemapParams();

		size_t Width = 0;
		size_t Height = 0;
		GLuint TextureID = 0;

		friend class GLRenderingDevice;
	};
}