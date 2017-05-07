#include "EnginePCH.hpp"

#include "GLTextureResource.hpp"
#include "ResourceManager.hpp"
#include "SOIL\SOIL.h"

#include <GL/glew.h>


using namespace Poly;

//------------------------------------------------------------------------------
GLTextureResource::GLTextureResource(const String& path)
{
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	Image = SOIL_load_image(path.GetCStr(), &Width, &Height, 0, SOIL_LOAD_RGBA);
	if (Image == nullptr)
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