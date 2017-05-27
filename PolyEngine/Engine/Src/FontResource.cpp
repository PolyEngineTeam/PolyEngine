#include "EnginePCH.hpp"

#include "FontResource.hpp"

using namespace Poly;

static FT_Library gFreeTypeLibrary = nullptr;

static const size_t GLYPH_PADDING = 8;

FontResource::FontResource(const String& path)
{
	if (!gFreeTypeLibrary)
	{
		FT_Error err = FT_Init_FreeType(&gFreeTypeLibrary);
		ASSERTE(err == FT_Err_Ok, "Freetype initialization failed!");
	}
	FontPath = path;

	gConsole.LogDebug("Font: {} loaded sucesfully!", path);
}

FontResource::~FontResource()
{
	for (auto kv : Faces)
	{
		FT_Done_Face(kv.second.FTFace);
		glDeleteTextures(1, &kv.second.TextureID);
	}
	//TODO release freetype library?
}

const FontResource::FontFace& FontResource::GetFace(size_t height) const
{
	auto it = Faces.find(height);
	if (it == Faces.end())
	{
		LoadFace(height);
		it = Faces.find(height);
	}
	ASSERTE(Faces.find(height) != Faces.end(), "Face not loaded!?");
	return it->second;
}

void Poly::FontResource::LoadFace(size_t height) const
{
	if (Faces.find(height) != Faces.end())
		return; // Already loaded

	// create face in map
	FontFace& face = Faces[height];

	// Load font face
	FT_Error err = FT_New_Face(gFreeTypeLibrary, FontPath.GetCStr(), 0, &face.FTFace);
	if (err != FT_Err_Ok)
		throw ResourceLoadFailedException();
	err = FT_Set_Pixel_Sizes(face.FTFace, 0, height);
	if (err != FT_Err_Ok)
		throw ResourceLoadFailedException();

	// Load only ASCII for now
	struct GlyphSize
	{
		char Glyph;
		unsigned width, height;
	};

	// Establish sizes of all glyphs
	Dynarray<GlyphSize> glyphSizes;
	glyphSizes.Reserve(128);
	for (size_t c = 0; c < 128; c++)
	{
		// Load character glyph 
		FT_Error err = FT_Load_Char(face.FTFace, c, FT_LOAD_RENDER);
		if (err != FT_Err_Ok)
		{
			gConsole.LogError("Glyph {} of size {} failed to load", (char)c, height);
			continue;
		}

		//if(face.FTFace->glyph->bitmap.width > 0 && face.FTFace->glyph->bitmap.rows > 0)
			glyphSizes.PushBack(GlyphSize{(char)c, face.FTFace->glyph->bitmap.width, face.FTFace->glyph->bitmap.rows });
	}

	// sort them by their area
	std::sort(glyphSizes.Begin(), glyphSizes.End(), [](const GlyphSize& g1, const GlyphSize& g2) { return g1.height * g1.width > g2.height * g2.width; });

	// Estimate texture height for constant width
	const size_t TEXTURE_WIDTH = 512;
	size_t estimatedTextureHeight = 0;
	size_t maxLastRowHeight = 0;
	size_t currRowLen = 0;
	for (const GlyphSize& glyphSize : glyphSizes)
	{

		
		currRowLen += glyphSize.width;
		if (currRowLen <= TEXTURE_WIDTH)
		{
			//Fits
			if (maxLastRowHeight < glyphSize.height)
				maxLastRowHeight = glyphSize.height;
		}
		else
		{
			//Next row
			ASSERTE(maxLastRowHeight > 0, "Texture packing for font failed!");
			currRowLen = 0;
			estimatedTextureHeight += maxLastRowHeight;
			maxLastRowHeight = 0;
		}
	}

	// Make sure it's power of two
	size_t power = 1;
	while (power < estimatedTextureHeight)
		power <<= 1;
	estimatedTextureHeight = power;
	ASSERTE(estimatedTextureHeight > 0, "Texture packing for font failed!");

	// Create texture 2D for the face
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &face.TextureID);
	glBindTexture(GL_TEXTURE_2D, face.TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, TEXTURE_WIDTH, estimatedTextureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Store all glyphs in it and maintain the references in Characters map
	currRowLen = 0;
	maxLastRowHeight = 0;
	size_t currTextureHeight = 0;
	for (const GlyphSize& glyphSize : glyphSizes)
	{		
		FT_Error err = FT_Load_Char(face.FTFace, glyphSize.Glyph, FT_LOAD_RENDER);
		ASSERTE(err == FT_Err_Ok, "Glyph loading failed!");

		currRowLen += glyphSize.width + GLYPH_PADDING;
		
		size_t xoffset = 0;
		size_t yoffset = 0;
		if (currRowLen <= TEXTURE_WIDTH)
		{
			//Fits
			if (maxLastRowHeight < glyphSize.height + GLYPH_PADDING)
				maxLastRowHeight = glyphSize.height + GLYPH_PADDING;

			xoffset = currRowLen - glyphSize.width - GLYPH_PADDING;
		}
		else
		{
			//Next row
			currRowLen = glyphSize.width;
			currTextureHeight += maxLastRowHeight;
			maxLastRowHeight = glyphSize.height;
			xoffset = 0;
		}
		yoffset = currTextureHeight;

		FontFace::FontGlyph glyph;
		glyph.TextureUV[0] = Vector((float)xoffset / (float)TEXTURE_WIDTH, (float)yoffset / (float)estimatedTextureHeight, 0);
		glyph.TextureUV[1] = Vector((float)(xoffset + glyphSize.width) / (float)TEXTURE_WIDTH, (float)(yoffset + glyphSize.height) / (float)estimatedTextureHeight, 0);
		glyph.Size = Vector(glyphSize.width, glyphSize.height, 0);
		glyph.Bearing = Vector(face.FTFace->glyph->bitmap_left, face.FTFace->glyph->bitmap_top, 0);
		glyph.Advance = (float)face.FTFace->glyph->advance.x / 64.0f;

		face.Characters.insert(std::pair<char, FontFace::FontGlyph>(glyphSize.Glyph, glyph));
		ASSERTE(face.FTFace->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY, "");
		glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, glyphSize.width, glyphSize.height, GL_RED, GL_UNSIGNED_BYTE, face.FTFace->glyph->bitmap.buffer);
	}

	gConsole.LogDebug("Face of size {} for font {} loaded sucessfully! Texture size: {} x {}", height, FontPath, TEXTURE_WIDTH, estimatedTextureHeight);
}
