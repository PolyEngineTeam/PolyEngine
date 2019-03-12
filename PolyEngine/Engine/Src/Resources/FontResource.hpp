#pragma once

#include <Defines.hpp>
#include <Math/Vector.hpp>
#include <Resources/ResourceBase.hpp>
#include <Resources/TextureResource.hpp>

struct FT_FaceRec_;
typedef struct FT_FaceRec_* FT_Face;

typedef unsigned int GLuint;

namespace Poly
{
	class ENGINE_DLLEXPORT FontResource : public ResourceBase
	{
	public:
		class FontFace
		{
		public:
			struct FontGlyph
			{
				Vector TextureUV[2];  // texture uv-s
				Vector Size;		  // Size of glyph
				Vector Bearing;       // Offset from baseline to left/top of glyph
				float Advance;       // Offset to advance to next glyph
			};
			
			std::unique_ptr<ITextureDeviceProxy> TextureProxy;
			FT_Face FTFace;
			std::map<char, FontGlyph> Characters;
		};

		FontResource(const String& path);
		virtual ~FontResource();

		const FontFace& GetFace(size_t height) const;

		void LoadFace(size_t height) const;
		void LoadFaces(std::initializer_list<size_t> list) const
		{
			for (size_t height : list)
				LoadFace(height);
		}
	private:
		String FontPath;
		mutable std::unordered_map<size_t, FontFace> Faces;
	};
}