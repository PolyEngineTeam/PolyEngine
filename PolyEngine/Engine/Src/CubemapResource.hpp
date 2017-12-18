#pragma once

#include "ResourceBase.hpp"

typedef unsigned int GLuint;

namespace Poly 
{
	class ITextureDeviceProxy;

	class ENGINE_DLLEXPORT CubemapResource : public ResourceBase
	{
	public:
		CubemapResource(const Dynarray<String> path);
		~CubemapResource() override;

		unsigned char* LoadImage(const String& path);
		unsigned char* GetImage(int side) const { return Images[side]; }
		int GetWidth() const { return Width; }
		int GetHeight() const { return Height; }
		int GetChannels() const { return Channels; }

		const ICubemapDeviceProxy* GetTextureProxy() const { return TextureProxy.get(); }
	private:
		std::unique_ptr<ICubemapDeviceProxy> TextureProxy;
		Dynarray<unsigned char*> Images;
		int Width;
		int Height;
		int Channels;
	};
}

