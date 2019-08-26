#pragma once

#include <pe/Defines.hpp>
#include <Common/GLUtils.hpp>

namespace Poly
{
	class GLCubemapDeviceProxy : public ICubemapDeviceProxy
	{
	public:
		GLCubemapDeviceProxy(size_t width, size_t height);
		virtual ~GLCubemapDeviceProxy();
		
		void SetContentHDR(const eCubemapSide side, const float* data) override;

		GLuint GetTextureID() const { return TextureID; }

	private:
		static GLenum GetEnumFromCubemapSide(eCubemapSide type);

		void InitCubemapParams();

		size_t Width = 0;
		size_t Height = 0;
		GLuint TextureID = 0;
	};
}