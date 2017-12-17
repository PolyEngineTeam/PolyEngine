#include "GLTextureDeviceProxy.hpp"
#include "GLUtils.hpp"
#include "GLCubemapDeviceProxy.hpp"

using namespace Poly;

GLCubemapDeviceProxy::GLCubemapDeviceProxy(size_t width, size_t height)
	: Width(width), Height(height)
{
	InitCubemapParams();
}

GLCubemapDeviceProxy::~GLCubemapDeviceProxy()
{
	if(TextureID > 0)
	{
		glDeleteTextures(1, &TextureID);
	}
}

void GLCubemapDeviceProxy::InitCubemapParams()
{
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GLCubemapDeviceProxy::SetContent(unsigned int side, const unsigned char* data)
{
	ASSERTE(Width > 0 && Height > 0, "Invalid arguments!");
	ASSERTE(TextureID > 0, "Texture is invalid!");
	ASSERTE(data, "Data pointer is nullptr!");
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);
	
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, GL_RGB, (GLsizei)Width, (GLsizei)Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERR();
}

