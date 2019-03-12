#include <PolyRenderingDeviceGLPCH.hpp>

#include <Proxy/GLTextureDeviceProxy.hpp>
#include <Common/GLUtils.hpp>

using namespace Poly;

static GLenum GetFormat(size_t channels) noexcept
{
	switch (channels)
	{
	case 1:
		return GL_RED;
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	default:
		ASSERTE(false, "Invalid Channels!");
	}
	return 0;
}

GLTextureDeviceProxy::GLTextureDeviceProxy(size_t width, size_t height, eRenderTargetType internalUsage, GLuint internalFormat)
	: Width(width), Height(height),
	Usage(eTextureUsageType::RENDER_TARGET), RenderTargetType(internalUsage)
{
	InitTextureParams();
}

GLTextureDeviceProxy::GLTextureDeviceProxy(size_t width, size_t height, size_t channels, eTextureUsageType usage)
	: Width(width), Height(height), Channels(channels),
	Usage(usage), RenderTargetType(eRenderTargetType::NONE)
{
	InitTextureParams();
}

GLTextureDeviceProxy::~GLTextureDeviceProxy()
{
	if(TextureID > 0)
		glDeleteTextures(1, &TextureID);
}

void GLTextureDeviceProxy::SetContent(const unsigned char* data)
{
	ASSERTE(Usage != eTextureUsageType::HDR, "Invalid texture usage type, unsigned char* is for LDR image data");
	ASSERTE(Width > 0 && Height > 0, "Invalid arguments!");
	ASSERTE(TextureID > 0, "Texture is invalid!");
	ASSERTE(data, "Data pointer is nullptr!");

	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, Format, GL_UNSIGNED_BYTE, data);

	if (Usage != eTextureUsageType::FONT)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void GLTextureDeviceProxy::SetContentHDR(const float* data)
{
	ASSERTE(Usage == eTextureUsageType::HDR, "Invalid texture usage type, float* is for HDR image data");
	ASSERTE(Width > 0 && Height > 0, "Invalid arguments!");
	ASSERTE(TextureID > 0 , "Texture is invalid!");
	ASSERTE(data, "Data pointer is nullptr!");

	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, Format, GL_FLOAT, data);

	if (Usage != eTextureUsageType::FONT)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void GLTextureDeviceProxy::SetSubContent(size_t width, size_t height,
	size_t offsetX, size_t offsetY, const unsigned char* data)
{
	ASSERTE(Usage != eTextureUsageType::HDR, "Invalid texture usage type, unsigned char* is for LDR image data");
	ASSERTE(width + offsetX <= Width && height + offsetY <= Height && width > 0 && height > 0, "Invalid arguments!");
	ASSERTE(data, "Data pointer is nullptr!");

	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, (GLint)offsetX, (GLint)offsetY, (GLsizei)width, (GLsizei)height, Format, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void GLTextureDeviceProxy::Resize(const ScreenSize& size)
{
	Width = size.Width;
	Height = size.Height;

	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	if (RenderTargetType == eRenderTargetType::DEPTH_ATTACHEMENT)
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	if (RenderTargetType == eRenderTargetType::COLOR_ATTACHEMENT)
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, GL_RGBA, GL_FLOAT, nullptr);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, InternalFormat, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void GLTextureDeviceProxy::InitTextureParams()
{
	ASSERTE(Width > 0 && Height > 0, "Invalid arguments!");
	if (TextureID > 0)
	{
		return;
	}

	// gConsole.LogInfo("GLTextureDeviceProxy::InitTextureParams usage: {}", (int)Usage);

	switch (Usage)
	{
		case eTextureUsageType::ALBEDO:
		case eTextureUsageType::EMISSIVE:
			InitTextureGamma();
			break;
		case eTextureUsageType::AMBIENT_OCCLUSION:
		case eTextureUsageType::METALLIC:
		case eTextureUsageType::ROUGHNESS:
			InitTextureLinear();
			break;
		case eTextureUsageType::NORMAL:
			InitTextureNormal();
			break;
		case eTextureUsageType::HDR:
			InitTextureHDR();
			break;
		case eTextureUsageType::FONT:
			InitTextureFont();
			break;
		case eTextureUsageType::RENDER_TARGET:
			InitTextureRenderTarget();
			break;
		default:
			ASSERTE(false, "Uknown eTextureUsageType!");
	}
}

void GLTextureDeviceProxy::InitTextureHDR()
{
	glGenTextures(1, &TextureID);

	if (TextureID <= 0)
		throw RenderingDeviceProxyCreationFailedException();

	glBindTexture(GL_TEXTURE_2D, TextureID);
	InternalFormat = GL_RGB16F;
	Format = GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void GLTextureDeviceProxy::InitTextureGamma()
{
	glGenTextures(1, &TextureID);

	if (TextureID <= 0)
		throw RenderingDeviceProxyCreationFailedException();

	glBindTexture(GL_TEXTURE_2D, TextureID);
	InternalFormat = Channels > 3 ? GL_SRGB8_ALPHA8 : GL_SRGB8;
	Format = Channels > 3 ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, Format, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void GLTextureDeviceProxy::InitTextureLinear()
{
	glGenTextures(1, &TextureID);

	if (TextureID <= 0)
		throw RenderingDeviceProxyCreationFailedException();

	glBindTexture(GL_TEXTURE_2D, TextureID);
	switch (Channels)
	{
		case 1:
			InternalFormat = GL_R8;
			break;
		case 3:
			InternalFormat = GL_RGB8;
			break;
		case 4:
			InternalFormat = GL_RGBA8;
			break;
		default:
			ASSERTE(false, "Invalid Channels!");
	}
	Format = GetFormat(Channels);

	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, Format, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void GLTextureDeviceProxy::InitTextureRenderTarget()
{
	glGenTextures(1, &TextureID);

	if (TextureID <= 0)
		throw RenderingDeviceProxyCreationFailedException();

	glBindTexture(GL_TEXTURE_2D, TextureID);

	switch (RenderTargetType)
	{
		case eRenderTargetType::COLOR_ATTACHEMENT:
			InternalFormat = Channels > 3 ? GL_RGBA8 : GL_RGB8;
			Format = Channels > 3 ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, Format, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;

		case eRenderTargetType::DEPTH_ATTACHEMENT:
			InternalFormat = GL_DEPTH_COMPONENT;
			Format = GL_DEPTH_COMPONENT;
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, Format, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;

		default:
			ASSERTE(false, "Invalid RenderTargetType!");
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void GLTextureDeviceProxy::InitTextureNormal()
{
	glGenTextures(1, &TextureID);

	if (TextureID <= 0)
		throw RenderingDeviceProxyCreationFailedException();

	glBindTexture(GL_TEXTURE_2D, TextureID);
	InternalFormat = GL_RGB;
	Format = GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, Format, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}

void GLTextureDeviceProxy::InitTextureFont()
{
	glGenTextures(1, &TextureID);

	if (TextureID <= 0)
		throw RenderingDeviceProxyCreationFailedException();

	glBindTexture(GL_TEXTURE_2D, TextureID);
	InternalFormat = GL_R8;
	Format = GL_RED;
	glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, Format, GL_UNSIGNED_BYTE, nullptr);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERR();
}