#include "EnginePCH.hpp"

#include "Resources/TextureResource.hpp"
#include "Resources/ResourceManager.hpp"

using namespace Poly;

TextureResource::TextureResource(const String& path, eTextureUsageType usage)
{
	gConsole.LogInfo("TextureResource::TextureResource path: {} usage: {}", path, (int)usage);

	float* Image = LoadImage(path.GetCStr(), &Width, &Height, &Channels);
	
	gConsole.LogInfo("TextureResource::TextureResource loaded width: {}, height: {}, channels: {}", Width, Height, Channels);

	TextureProxy = gEngine->GetRenderingDevice()->CreateTexture(Width, Height, Channels, usage);
	TextureProxy->SetContent(Image);

	FreeImage(Image);
}

eTextureDataFormat TextureResource::GetFormat(int channels)
{
	switch (channels)
	{
		case 1:
			return eTextureDataFormat::RED;
		case 3:
			return eTextureDataFormat::RGB;
		case 4:
			return eTextureDataFormat::RGBA;
		default:
			ASSERTE(false, "TextureResource::GetFormat uknown channels value");
			throw ResourceLoadFailedException();
	}
}

TextureResource::~TextureResource()
{
}