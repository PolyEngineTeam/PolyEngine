#pragma once

#include <pe/Defines.hpp>
#include <Common/GLUtils.hpp>

namespace Poly
{
	struct DebugRenderingBuffers : public BaseObject<>
	{
		DebugRenderingBuffers();
		~DebugRenderingBuffers();

		GLuint VAO = 0;
		GLuint VBO = 0;
	};
}