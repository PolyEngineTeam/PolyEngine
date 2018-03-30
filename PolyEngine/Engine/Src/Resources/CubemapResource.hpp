#pragma once

#include "Resources/ResourceBase.hpp"

typedef unsigned int GLuint;

namespace Poly 
{
	class ITextureDeviceProxy;

	class ENGINE_DLLEXPORT CubemapResource : public ResourceBase
	{
	public:
		CubemapResource(const EnumArray<String, eCubemapSide> paths);
		~CubemapResource() override;
		
		unsigned char* GetImage(const eCubemapSide side) const { return Images[side]; }
		int GetWidth() const { return Width; }
		int GetHeight() const { return Height; }
		int GetChannels() const { return Channels; }

		const ICubemapDeviceProxy* GetTextureProxy() const { return TextureProxy.get(); }
	private:
		std::unique_ptr<ICubemapDeviceProxy> TextureProxy;
		EnumArray<unsigned char*, eCubemapSide> Images;
		int Width;
		int Height;
		int Channels;

		unsigned char* LoadImage(const String& path);
	};
}

