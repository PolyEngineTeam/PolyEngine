#include "GLTextureDeviceProxy.hpp"
#include "GLUtils.hpp"

using namespace Poly;

//---------------------------------------------------------------
static GLenum GetGLDataFormat(eTextureDataFormat format) noexcept
{
	switch (format)
	{
	case Poly::eTextureDataFormat::RED:
		return GL_RED;
	case Poly::eTextureDataFormat::RGB:
		return GL_RGB;
	case Poly::eTextureDataFormat::RGBA:
		return GL_RGBA;
	default:
		ASSERTE(false, "Invalid format!");
	}
	return 0;
}

//---------------------------------------------------------------
static GLenum GetGLInternalFormat(eTextureUsageType usage) noexcept
{
	switch (usage)
	{
	case Poly::eTextureUsageType::DIFFUSE:
		return GL_RGBA;
	case Poly::eTextureUsageType::FONT:
		return GL_RED;
	default:
		ASSERTE(false, "Invalid usage!");
	}
	return 0;
}

//-------------------------------------------------------------- -
Poly::GLTextureDeviceProxy::GLTextureDeviceProxy(size_t width, size_t height, eInternalTextureUsageType internalUsage, GLuint internalFormat)
	: Width(width), Height(height), InternalFormat(internalFormat), InternalUsage(internalUsage)
{
	InitTextureParams();
}

//---------------------------------------------------------------
GLTextureDeviceProxy::GLTextureDeviceProxy(size_t width, size_t height, eTextureUsageType usage)
	: GLTextureDeviceProxy(width, height, eInternalTextureUsageType::NONE, GetGLInternalFormat(usage))
{
	Usage = usage;
	InitTextureParams();
}

//---------------------------------------------------------------
GLTextureDeviceProxy::~GLTextureDeviceProxy()
{
	if(TextureID > 0)
		glDeleteTextures(1, &TextureID);
}

//---------------------------------------------------------------
void GLTextureDeviceProxy::SetContent(eTextureDataFormat format, const unsigned char* data)
{
	ASSERTE(Width > 0 && Height > 0, "Invalid arguments!");
	ASSERTE(TextureID > 0 , "Texture is invalid!");
	ASSERTE(data, "Data pointer is nullptr!");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, GetGLDataFormat(format), GL_UNSIGNED_BYTE, data);

	if (Usage != eTextureUsageType::FONT)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

//---------------------------------------------------------------
void GLTextureDeviceProxy::SetSubContent(size_t width, size_t height,
	size_t offsetX, size_t offsetY, eTextureDataFormat format, const unsigned char* data)
{
	ASSERTE(width + offsetX <= Width && height + offsetY <= Height && width > 0 && height > 0, "Invalid arguments!");
	ASSERTE(data, "Data pointer is nullptr!");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, (GLint)offsetX, (GLint)offsetY, (GLsizei)width, (GLsizei)height, GetGLDataFormat(format), GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void Poly::GLTextureDeviceProxy::Resize(const ScreenSize& size)
{
	Width = size.Width;
	Height = size.Height;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	if (InternalUsage == eInternalTextureUsageType::DEPTH_ATTACHEMENT)
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	if (InternalUsage == eInternalTextureUsageType::COLOR_ATTACHEMENT)
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, GL_RGBA, GL_FLOAT, nullptr);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void Poly::GLTextureDeviceProxy::InitTextureParams()
{
	ASSERTE(Width > 0 && Height > 0, "Invalid arguments!");

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &TextureID);

	if (TextureID <= 0)
		throw RenderingDeviceProxyCreationFailedException();

	if (Usage == eTextureUsageType::FONT)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glBindTexture(GL_TEXTURE_2D, TextureID);

	if(InternalUsage == eInternalTextureUsageType::DEPTH_ATTACHEMENT)
	{
		// glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (GLsizei)Width, (GLsizei)Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	}
	else if (InternalUsage == eInternalTextureUsageType::COLOR_ATTACHEMENT)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, GL_RGBA, GL_FLOAT, nullptr);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, GL_RGBA, GL_FLOAT, nullptr);
	}

	if (InternalUsage == eInternalTextureUsageType::COLOR_ATTACHEMENT || InternalUsage == eInternalTextureUsageType::DEPTH_ATTACHEMENT)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else if (Usage == eTextureUsageType::FONT)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 16);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}
