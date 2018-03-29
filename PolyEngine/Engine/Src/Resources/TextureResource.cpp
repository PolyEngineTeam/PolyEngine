#include "EnginePCH.hpp"

#include "TextureResource.hpp"
#include "ResourceManager.hpp"
#include "SOIL/SOIL.h"

using namespace Poly;

//------------------------------------------------------------------------------
TextureResource::TextureResource(const String& path, eTextureUsageType textureUsageType)
{
	Channels = 4;

	int FileChannels;
	Image = SOIL_load_image(path.GetCStr(), &Width, &Height, &FileChannels, SOIL_LOAD_AUTO);
	if (Image == nullptr)
	{
		throw ResourceLoadFailedException();
	}

	// Flip Y axis
	int rowSize = Width*FileChannels;
	static Dynarray<unsigned char> row;
	row.Resize(rowSize);
	for (int i = 0; i < Height/2; ++i) {
		memcpy(row.GetData(), Image + ((Height - i - 1) * Width*FileChannels), sizeof(unsigned char) * rowSize);
		memcpy(Image + ((Height - i - 1) * Width*FileChannels), Image + (i * Width*FileChannels), sizeof(unsigned char) * rowSize);
		memcpy(Image + (i * Width*FileChannels), row.GetData(), sizeof(unsigned char) * rowSize);
	}

	TextureProxy = gEngine->GetRenderingDevice()->CreateTexture(Width, Height, textureUsageType); //HACK, remove deffise from here

	switch(FileChannels)
	{
		case 1:
			TextureProxy->SetContent(eTextureDataFormat::RED, Image);
			break;
		case 3:
			TextureProxy->SetContent(eTextureDataFormat::RGB, Image);
			break;
		case 4:
			TextureProxy->SetContent(eTextureDataFormat::RGBA, Image);
			break;
	}
}

//-----------------------------------------------------------------------------
TextureResource::~TextureResource()
{
	SOIL_free_image_data(Image);
}
