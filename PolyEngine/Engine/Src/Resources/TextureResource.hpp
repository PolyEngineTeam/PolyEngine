#pragma once

#include "Resources/ResourceBase.hpp"

typedef unsigned int GLuint;

namespace Poly 
{
	class ITextureDeviceProxy;
	enum class eTextureUsageType;

	class ENGINE_DLLEXPORT TextureResource : public ResourceBase
	{
	public:
		TextureResource(const String& path, eTextureUsageType textureUsageType);
		~TextureResource() override;

		// float* GetImage() const { return Image; }
		int GetWidth() const { return Width; }
		int GetHeight() const { return Height; }
		int GetChannels() const { return Channels; }

		const ITextureDeviceProxy* GetTextureProxy() const { return TextureProxy.get(); }

	private:
		std::unique_ptr<ITextureDeviceProxy> TextureProxy;
		// float* Image;
		int Width = 0;
		int Height = 0;
		int Channels = 0;

		// eTextureDataFormat GetFormat(int channels);
	};
}

