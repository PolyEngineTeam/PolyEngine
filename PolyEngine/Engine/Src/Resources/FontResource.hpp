#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/Vector.hpp>
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
				core::math::Vector TextureUV[2];  // texture uv-s
				core::math::Vector Size;		  // Size of glyph
				core::math::Vector Bearing;       // Offset from baseline to left/top of glyph
				float Advance;       // Offset to advance to next glyph
			};
			
			std::unique_ptr<ITextureDeviceProxy> TextureProxy;
			FT_Face FTFace;
			std::map<char, FontGlyph> Characters;
		};

		FontResource(const core::storage::String& path);
		virtual ~FontResource();

		const FontFace& GetFace(size_t height) const;

		void LoadFace(size_t height) const;
		void LoadFaces(std::initializer_list<size_t> list) const
		{
			for (size_t height : list)
				LoadFace(height);
		}
	private:
		core::storage::String FontPath;
		mutable std::unordered_map<size_t, FontFace> Faces;
	};
}