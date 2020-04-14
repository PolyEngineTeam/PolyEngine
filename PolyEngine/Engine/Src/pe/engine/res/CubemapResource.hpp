#pragma once

#include <pe/Defines.hpp>
#include <pe/engine/res/ResourceBase.hpp>
#include <pe/api/rendering/IRenderingDevice.hpp>

typedef unsigned int GLuint;

namespace pe::engine::res 
{
	class ITextureDeviceProxy;

	class ENGINE_DLLEXPORT CubemapResource : public ResourceBase
	{
	public:
		CubemapResource(const ::pe::core::utils::EnumArray<::pe::core::storage::String, api::rendering::eCubemapSide> paths);
		~CubemapResource() override;
		
		float* GetImage(const api::rendering::eCubemapSide side) const { return Images[side]; }
		int GetWidth() const { return Width; }
		int GetHeight() const { return Height; }
		int GetChannels() const { return Channels; }

		const api::rendering::ICubemapDeviceProxy* GetTextureProxy() const { return TextureProxy.get(); }
	private:
		std::unique_ptr<api::rendering::ICubemapDeviceProxy> TextureProxy;
		::pe::core::utils::EnumArray<float*, api::rendering::eCubemapSide> Images;
		int Width;
		int Height;
		int Channels;
	};
}

