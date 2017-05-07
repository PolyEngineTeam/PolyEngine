#pragma once

#include "ResourceBase.hpp"

typedef unsigned int GLuint;

namespace Poly 
{
	class ENGINE_DLLEXPORT GLTextureResource : public ResourceBase
	{
	public:
		GLTextureResource(const String& path);
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

