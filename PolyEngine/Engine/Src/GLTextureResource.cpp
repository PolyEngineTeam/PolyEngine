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
	Image = SOIL_load_image(path.c_str(), &Width, &Height, 0, SOIL_LOAD_RGBA);
	ASSERTE(0 != Image, SOIL_last_result());
	if (0 == Image)
	{
		throw ResourceLoadFailedException();
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