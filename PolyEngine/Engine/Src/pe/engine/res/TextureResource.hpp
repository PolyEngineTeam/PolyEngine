#pragma once

#include <pe/Defines.hpp>
#include <pe/engine/res/ResourceBase.hpp>
#include <pe/api/rendering/IRenderingDevice.hpp>

typedef unsigned int GLuint;

namespace pe::api::rendering { enum class eTextureUsageType; }

namespace pe::engine::res 
{
	class ENGINE_DLLEXPORT TextureResource : public ResourceBase
	{
	public:
		TextureResource(const ::pe::core::storage::String& path, api::rendering::eTextureUsageType textureUsageType);
		~TextureResource() override;

		int GetWidth() const { return Width; }
		int GetHeight() const { return Height; }
		int GetChannels() const { return Channels; }

		const api::rendering::ITextureDeviceProxy* GetTextureProxy() const { return TextureProxy.get(); }

	private:
		std::unique_ptr<api::rendering::ITextureDeviceProxy> TextureProxy;
		int Width = 0;
		int Height = 0;
		int Channels = 0;
	};
}

