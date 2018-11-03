#include <EnginePCH.hpp>

#include <Resources/TextureResource.hpp>
#include <Resources/ResourceManager.hpp>
#include <Rendering/IRenderingDevice.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

static int GetDesiredChannel(eTextureUsageType usage) noexcept
{
	switch (usage)
	{
	case eTextureUsageType::ALBEDO:
	case eTextureUsageType::EMISSIVE:
	case eTextureUsageType::RENDER_TARGET:
		return 4;
	case eTextureUsageType::AMBIENT_OCCLUSION:
	case eTextureUsageType::METALLIC:
	case eTextureUsageType::ROUGHNESS:
	case eTextureUsageType::NORMAL:
	case eTextureUsageType::HDR:
	case eTextureUsageType::FONT:
		return 3;
	default:
		ASSERTE(false, "Uknown eTextureUsageType!");
	}
	return 0;
}

TextureResource::TextureResource(const String& path, eTextureUsageType usage)
{
	gConsole.LogInfo("TextureResource::TextureResource path: {} usage: {}", path, (int)usage);

	int desiredChannels = GetDesiredChannel(usage);

	if (usage == eTextureUsageType::HDR) 
	{
		float* Image = LoadImageHDR(path.GetCStr(), &Width, &Height, &Channels);
	
		gConsole.LogInfo("TextureResource::TextureResource loaded width: {}, height: {}, channels: {}", Width, Height, Channels);

		TextureProxy = gEngine->GetRenderingDevice()->CreateTexture(Width, Height, desiredChannels, usage);
		TextureProxy->SetContentHDR(Image);
		FreeImageHDR(Image);
	}
	else
	{
		unsigned char* Image = LoadImage(path.GetCStr(), &Width, &Height, &Channels, desiredChannels);

		gConsole.LogInfo("TextureResource::TextureResource loaded width: {}, height: {}, channels: {}, desiredChannels: {}",
			Width, Height, Channels, desiredChannels);

		TextureProxy = gEngine->GetRenderingDevice()->CreateTexture(Width, Height, desiredChannels, usage);
		TextureProxy->SetContent(Image);
		FreeImage(Image);
	}
}

TextureResource::~TextureResource()
{
}
