#pragma once

#include "ResourceBase.hpp"

typedef unsigned int GLuint;

namespace Poly 
{
	class ITextureDeviceProxy;

	class ENGINE_DLLEXPORT TextureResource : public ResourceBase
	{
	public:
		TextureResource(const String& path, eTextureUsageType textureUsageType);
		~TextureResource() override;

		unsigned char* GetImage() const { return Image; }
		int GetWidth() const { return Width; }
		int GetHeight() const { return Height; }
		int GetChannels() const { return Channels; }

		const ITextureDeviceProxy* GetTextureProxy() const { return TextureProxy.get(); }
	private:
		std::unique_ptr<ITextureDeviceProxy> TextureProxy;
		unsigned char* Image;
		int Width;
		int Height;
		int Channels;
	};
}

