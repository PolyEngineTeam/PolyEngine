#pragma once

#include <pe/Defines.hpp>
#include <pe/core/math/Vector.hpp>
#include <pe/engine/res/ResourceBase.hpp>
#include <pe/engine/res/TextureResource.hpp>

struct FT_FaceRec_;
typedef struct FT_FaceRec_* FT_Face;

typedef unsigned int GLuint;

namespace pe::engine::res 
{
	class ENGINE_DLLEXPORT FontResource : public ResourceBase
	{
	public:
		class FontFace
		{
		public:
			struct FontGlyph
			{
				::pe::core::math::Vector TextureUV[2];  // texture uv-s
				::pe::core::math::Vector Size;		  // Size of glyph
				::pe::core::math::Vector Bearing;       // Offset from baseline to left/top of glyph
				float Advance;       // Offset to advance to next glyph
			};
			
			std::unique_ptr<api::rendering::ITextureDeviceProxy> TextureProxy;
			FT_Face FTFace;
			std::map<char, FontGlyph> Characters;
		};

		FontResource(const ::pe::core::storage::String& path);
		virtual ~FontResource();

		const FontFace& GetFace(size_t height) const;

		void LoadFace(size_t height) const;
		void LoadFaces(std::initializer_list<size_t> list) const
		{
			for (size_t height : list)
				LoadFace(height);
		}
	private:
		::pe::core::storage::String FontPath;
		mutable std::unordered_map<size_t, FontFace> Faces;
	};
}