#include "EnginePCH.hpp"

#include "Texture.hpp"
#include "SOIL.h"

using namespace Poly;


//------------------------------------------------------------------------------
Texture::Texture(const std::string& path)
{
	GLuint tex_2d = SOIL_load_OGL_texture
	(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
}