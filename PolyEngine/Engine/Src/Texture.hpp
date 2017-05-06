#pragma once

#include <GL/glew.h>

namespace Poly 
{
	class ENGINE_DLLEXPORT Texture : public ResourceBase
	{
	public:
		Texture(const std::string& path);

	private:
		GLuint Tex;
	};
}

