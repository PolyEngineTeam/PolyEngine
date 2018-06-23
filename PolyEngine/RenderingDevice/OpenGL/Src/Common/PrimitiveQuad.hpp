#pragma once

#include "Proxy/GLUtils.hpp"

namespace Poly
{
	struct PrimitiveQuad : public BaseObject<>
	{
		PrimitiveQuad();
		~PrimitiveQuad();
	
		GLuint VAO = 0;
		GLuint VBO = 0;
	};
}