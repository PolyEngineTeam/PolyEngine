#pragma once

#include <pe/Defines.hpp>
#include <Resources/ResourceBase.hpp>
#include <Rendering/IRenderingDevice.hpp>

typedef unsigned int GLuint;

namespace Poly 
{
	class ITextureDeviceProxy;

	class ENGINE_DLLEXPORT CubemapResource : public ResourceBase
	{
	public:
		CubemapResource(const ::pe::core::utils::EnumArray<::pe::core::storage::String, eCubemapSide> paths);
		~CubemapResource() override;
		
		float* GetImage(const eCubemapSide side) const { return Images[side]; }
		int GetWidth() const { return Width; }
		int GetHeight() const { return Height; }
		int GetChannels() const { return Channels; }

		const ICubemapDeviceProxy* GetTextureProxy() const { return TextureProxy.get(); }
	private:
		std::unique_ptr<ICubemapDeviceProxy> TextureProxy;
		::pe::core::utils::EnumArray<float*, eCubemapSide> Images;
		int Width;
		int Height;
		int Channels;
	};
}

