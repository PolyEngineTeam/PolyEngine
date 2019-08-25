#pragma once

#include <pe/Defines.hpp>
#include <Common/GLUtils.hpp>

namespace Poly
{
	struct PrimitiveCube : public ::pe::core::BaseObject<>
	{
		PrimitiveCube();
		~PrimitiveCube();
	
		GLuint VAO = 0;
		GLuint VBO = 0;
	};
}