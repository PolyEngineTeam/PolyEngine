#pragma once

#include "GLUtils.hpp"

namespace Poly
{
	struct PostprocessQuad : public BaseObject<>
	{
		PostprocessQuad();
		~PostprocessQuad();
	
		GLuint VAO = 0;
		GLuint VBO = 0;
	};
}