#include "EnginePCH.hpp"

#include "GLTextureResource.hpp"
#include "ResourceManager.hpp"
#include "SOIL\SOIL.h"

using namespace Poly;

//------------------------------------------------------------------------------
GLTextureResource::GLTextureResource(const std::string& path)
{
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	unsigned char* image = SOIL_load_image(path.c_str(), &Width, &Height, 0, SOIL_LOAD_RGB);
	ASSERTE(0 != image, SOIL_last_result());
	if (0 == image)
	{
		throw ResourceLoadFailedException();
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
GLTextureResource::~GLTextureResource()
{
	glDeleteTextures(1, &TextureID);
}