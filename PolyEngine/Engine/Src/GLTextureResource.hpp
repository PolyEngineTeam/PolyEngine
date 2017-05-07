#pragma once

#include <GL/glew.h>

namespace Poly 
{
	class ENGINE_DLLEXPORT GLTextureResource : public ResourceBase
	{
	public:
		GLTextureResource(const std::string& path);
		~GLTextureResource() override;

		GLuint GetID() const { return TextureID; }
		unsigned char* GetImage() const { return Image; }
		int GetWidth() const { return Width; }
		int GetHeight() const { return Height; }

	private:
		GLuint TextureID; 
		unsigned char* Image;
		int Width;
		int Height;
	};
}

