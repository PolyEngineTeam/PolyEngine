#pragma once

#include <GL/glew.h>

namespace Poly 
{
	class ENGINE_DLLEXPORT GLTextureResource : public ResourceBase
	{
	public:
		GLTextureResource(const std::string& path);
		~GLTextureResource() override;

		GLuint GetID() { return TextureID; }
		int GetWidth() { return Width; }
		int GetHeight() { return Height; }

	private:
		GLuint TextureID;
		int Width;
		int Height;
	};
}

