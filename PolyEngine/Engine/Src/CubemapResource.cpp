#include "EnginePCH.hpp"

#include "CubemapResource.hpp"
#include "ResourceManager.hpp"
#include "SOIL/SOIL.h"

using namespace Poly;

CubemapResource::CubemapResource(const EnumArray<String, eCubemapSide> paths)
{
	ASSERTE(paths.GetSize() == 6, "CubemapResource::CubemapResource parths need to have 6 elements");
	gConsole.LogInfo("CubemapResource::CubemapResource path:{}", paths[eCubemapSide::LEFT]);

	for (auto side : IterateEnum<eCubemapSide>())
	{
		String absolutePath = gAssetsPathConfig.GetAssetsPath(eResourceSource::ENGINE) + paths[side];
		Images[side] = LoadImage(absolutePath);
	}

	TextureProxy = gEngine->GetRenderingDevice()->CreateCubemap(Width, Height);

	for (auto side : IterateEnum<eCubemapSide>())
	{
		TextureProxy->SetContent(side, Images[side]);
	}
}

unsigned char* CubemapResource::LoadImage(const String& path)
{
	unsigned char* image;
	Channels = 3;

	int fileChannels;
	int fileWidth;
	int fileHeight;
	image = SOIL_load_image(path.GetCStr(), &fileWidth, &fileHeight, &fileChannels, SOIL_LOAD_RGB);
	if (image == nullptr)
	{
		throw ResourceLoadFailedException();
	}

	gConsole.LogInfo("CubemapResource::LoadImage {}x{}:{} path:{}", fileWidth, fileHeight, fileChannels, path);

	Width = std::max(Width, fileWidth);
	Height = std::max(Height, fileHeight);

	return image;
}

CubemapResource::~CubemapResource()
{
	gConsole.LogInfo("CubemapResource::~CubemapResource");

	for (auto side : IterateEnum<eCubemapSide>())
	{
		SOIL_free_image_data(Images[side]);
	}
}