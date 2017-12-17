#include "EnginePCH.hpp"

#include "CubemapResource.hpp"
#include "ResourceManager.hpp"
#include "SOIL/SOIL.h"

using namespace Poly;

CubemapResource::CubemapResource(const String& path)
{
	gConsole.LogInfo("CubemapResource::CubemapResource path:{}", path);

	String absolutePath = gAssetsPathConfig.GetAssetsPath(eResourceSource::ENGINE) + path;

	Dynarray<String> suffixes(6);
	suffixes.PushBack("_ft.jpg");
	suffixes.PushBack("_bk.jpg");
	suffixes.PushBack("_lt.jpg");
	suffixes.PushBack("_rt.jpg");
	suffixes.PushBack("_up.jpg");
	suffixes.PushBack("_dn.jpg");

	Images.Reserve(6);
	for (int i = 0; i < suffixes.GetSize(); ++i) 
	{
		Images.PushBack(LoadImage(absolutePath + suffixes[i]));
	}

	TextureProxy = gEngine->GetRenderingDevice()->CreateCubemap(Width, Height);

	for (int i = 0; i < suffixes.GetSize(); ++i)
	{
		TextureProxy->SetContent(i, Images[i]);
	}
}

// TODO: not DRY, see TextureResource
unsigned char* CubemapResource::LoadImage(const String& path)
{
	gConsole.LogInfo("CubemapResource::LoadImage path:{}", path);

	unsigned char* image;
	Channels = 4;

	int FileChannels;
	image = SOIL_load_image(path.GetCStr(), &Width, &Height, &FileChannels, SOIL_LOAD_RGBA);
	if (image == nullptr)
	{
		throw ResourceLoadFailedException();
	}

	// Flip Y axis
	int rowSize = Width*Channels;
	static Dynarray<unsigned char> row;
	row.Resize(rowSize);
	for (int i = 0; i < Height / 2; ++i) {
		memcpy(row.GetData(), image + ((Height - i - 1) * Width*Channels), sizeof(unsigned char) * rowSize);
		memcpy(image + ((Height - i - 1) * Width*Channels), image + (i * Width*Channels), sizeof(unsigned char) * rowSize);
		memcpy(image + (i * Width*Channels), row.GetData(), sizeof(unsigned char) * rowSize);
	}

	return image;
}

CubemapResource::~CubemapResource()
{
	gConsole.LogInfo("CubemapResource::~CubemapResource");

	for(int i = 0; i < Images.GetSize(); ++i)
	{
		SOIL_free_image_data(Images[i]);
	}
}