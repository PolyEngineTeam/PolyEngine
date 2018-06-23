#pragma once

#include "Proxy/GLUtils.hpp"

namespace Poly
{
	struct PrimitiveCube : public BaseObject<>
	{
		PrimitiveCube();
		~PrimitiveCube();
	
		GLuint VAO = 0;
		GLuint VBO = 0;
	};
}