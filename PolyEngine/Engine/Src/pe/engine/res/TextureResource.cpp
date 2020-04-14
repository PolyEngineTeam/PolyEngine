#include <pe/engine/EnginePCH.hpp>

#include <pe/engine/res/TextureResource.hpp>
#include <pe/engine/res/ResourceManager.hpp>
#include <pe/api/rendering/IRenderingDevice.hpp>
#include <pe/api/ecs/Scene.hpp>

namespace pe::engine::res {

static int GetDesiredChannel(api::rendering::eTextureUsageType usage) noexcept
{
	switch (usage)
	{
	case api::rendering::eTextureUsageType::ALBEDO:
	case api::rendering::eTextureUsageType::EMISSIVE:
	case api::rendering::eTextureUsageType::RENDER_TARGET:
		return 4;
	case api::rendering::eTextureUsageType::AMBIENT_OCCLUSION:
	case api::rendering::eTextureUsageType::METALLIC:
	case api::rendering::eTextureUsageType::ROUGHNESS:
	case api::rendering::eTextureUsageType::NORMAL:
	case api::rendering::eTextureUsageType::HDR:
	case api::rendering::eTextureUsageType::FONT:
		return 3;
	default:
		ASSERTE(false, "Uknown eTextureUsageType!");
	}
	return 0;
}

TextureResource::TextureResource(const core::storage::String& path, api::rendering::eTextureUsageType usage)
{
	core::utils::gConsole.LogInfo("TextureResource::TextureResource path: {} usage: {}", path, (int)usage);

	int desiredChannels = GetDesiredChannel(usage);

	if (usage == api::rendering::eTextureUsageType::HDR) 
	{
		float* Image = LoadImageHDR(path.GetCStr(), &Width, &Height, &Channels);

		if (!Image)
			throw ResourceLoadFailedException();
	
		core::utils::gConsole.LogInfo("TextureResource::TextureResource loaded width: {}, height: {}, channels: {}", Width, Height, Channels);

		//TextureProxy = gEngine->GetRenderingDevice()->CreateTexture(Width, Height, desiredChannels, usage);
		TextureProxy->SetContentHDR(Image);
		FreeImageHDR(Image);
	}
	else
	{
		unsigned char* Image = LoadImage(path.GetCStr(), &Width, &Height, &Channels, desiredChannels);

		if (!Image)
			throw ResourceLoadFailedException();

		core::utils::gConsole.LogInfo("TextureResource::TextureResource loaded width: {}, height: {}, channels: {}, desiredChannels: {}",
			Width, Height, Channels, desiredChannels);

		//TextureProxy = gEngine->GetRenderingDevice()->CreateTexture(Width, Height, desiredChannels, usage);
		TextureProxy->SetContent(Image);
		FreeImage(Image);
	}
}

TextureResource::~TextureResource()
{
}

}