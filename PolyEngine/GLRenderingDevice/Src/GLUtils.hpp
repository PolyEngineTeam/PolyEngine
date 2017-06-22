#pragma once

#include <GL/glew.h>
#ifdef _WIN32
	#include <GL/wglew.h>
#elif defined(__linux__)
	#include <GL/glxew.h>
#elif
	#error "Unsupported platform :("
#endif

#include <Logger.hpp>

namespace Poly {
	//---------------------------------------------------------------
	inline const char* OpenGLErrorStr(GLenum err) noexcept
	{
		switch (err)
		{
			case GL_INVALID_OPERATION:				return "INVALID_OPERATION";
			case GL_INVALID_ENUM:					return "INVALID_ENUM";
			case GL_INVALID_VALUE:					return "INVALID_VALUE";
			case GL_OUT_OF_MEMORY:					return "OUT_OF_MEMORY";
			case GL_INVALID_FRAMEBUFFER_OPERATION:  return "INVALID_FRAMEBUFFER_OPERATION";
			default:								ASSERTE(false, "Unknown OpenGL error type!");
		}
	}

	//---------------------------------------------------------------
	inline void CheckGLError(const char *file, int line)
	{
		GLenum err = GL_NO_ERROR;
		while ((err = glGetError()) != GL_NO_ERROR)
			gConsole.LogError("{}:{} OpenGL error: [{}] {}", file, line, err, OpenGLErrorStr(err));
	}
}

#ifdef _DEBUG
	#define CHECK_GL_ERR() ::Poly::CheckGLError(__FILE__, __LINE__ )
#else
	#define CHECK_GL_ERR() (void)0
#endif // _DEBUG

