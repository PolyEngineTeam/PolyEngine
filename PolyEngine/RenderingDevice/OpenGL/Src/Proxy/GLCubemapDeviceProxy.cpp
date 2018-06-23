#include "Proxy/GLCubemapDeviceProxy.hpp"
#include "Proxy/GLTextureDeviceProxy.hpp"
#include "Common/GLUtils.hpp"

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

void GLCubemapDeviceProxy::SetContent(const eCubemapSide side, const float* data)
{
	ASSERTE(Width > 0 && Height > 0, "Invalid arguments!");
	ASSERTE(TextureID > 0, "Texture is invalid!");
	ASSERTE(data, "Data pointer is nullptr!");
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);
	
	glTexImage2D( GetEnumFromCubemapSide(side), 0, GL_RGB, (GLsizei)Width, (GLsizei)Height, 0, GL_RGB, GL_FLOAT, data);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERR();
}

GLenum GLCubemapDeviceProxy::GetEnumFromCubemapSide(eCubemapSide type)
{
	switch (type)
	{
		case eCubemapSide::RIGHT:	return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		case eCubemapSide::LEFT:	return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		case eCubemapSide::TOP:		return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		case eCubemapSide::DOWN:	return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		case eCubemapSide::BACK:	return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		case eCubemapSide::FRONT:	return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		default:
			ASSERTE(false, "Invalid type!");
			return -1;
	}
}
