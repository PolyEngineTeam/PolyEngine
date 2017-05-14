#include "EnginePCH.hpp"

#include "GLTextureResource.hpp"
#include "ResourceManager.hpp"
#include "SOIL/SOIL.h"

#include <GL/glew.h>


using namespace Poly;

//------------------------------------------------------------------------------
GLTextureResource::GLTextureResource(const String& path)
{

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 16);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	Image = SOIL_load_image(path.GetCStr(), &Width, &Height, &Channels, SOIL_LOAD_RGBA);
	if (Image == nullptr)
	{
		throw ResourceLoadFailedException();
	}

	// Flip Y axis
	int rowSize = Width*Channels;
	static Dynarray<unsigned char> row;
	row.Resize(rowSize);
	for (int i = 0; i < Height/2; ++i) {
		memcpy(row.GetData(), Image + ((Height - i - 1) * Width*Channels), sizeof(unsigned char) * rowSize);
		memcpy(Image + ((Height - i - 1) * Width*Channels), Image + (i * Width*Channels), sizeof(unsigned char) * rowSize);
		memcpy(Image + (i * Width*Channels), row.GetData(), sizeof(unsigned char) * rowSize);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
GLTextureResource::~GLTextureResource()
{
	SOIL_free_image_data(Image);
	glDeleteTextures(1, &TextureID);
}
