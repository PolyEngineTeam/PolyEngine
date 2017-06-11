#pragma once

#include <GL/glew.h>
#include <String.hpp>
#include <Logger.hpp>

namespace Poly {

	inline String ErrorStr(GLenum err) {
		String error;

		switch (err) {
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		return error;
	}

	inline void CheckGLError(const char *file, int line) {
		GLenum err = GL_NO_ERROR;
		while ((err = glGetError()) != GL_NO_ERROR) {
			gConsole.LogError("{}:{} OpenGL error: [{}] {}", file, line, err, ErrorStr(err));
		}
	}

}

#define CHECK_GL_ERR() Poly::CheckGLError(__FILE__, __LINE__ )