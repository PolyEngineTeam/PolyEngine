#include <EnginePCH.hpp>

#include <Resources/FontResource.hpp>
#include <ECS/Scene.hpp>

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
	for (auto& kv : Faces)
	{
		FT_Done_Face(kv.second.FTFace);
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
	err = FT_Set_Pixel_Sizes(face.FTFace, 0, (FT_UInt)height);
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
		FT_Error err = FT_Load_Char(face.FTFace, (FT_ULong)c, FT_LOAD_RENDER);
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


		currRowLen += glyphSize.width + GLYPH_PADDING;
		if (currRowLen <= TEXTURE_WIDTH)
		{
			//Fits
			if (maxLastRowHeight < glyphSize.height + GLYPH_PADDING)
				maxLastRowHeight = glyphSize.height + GLYPH_PADDING;
		}
		else
		{
			//Next row
			ASSERTE(maxLastRowHeight > 0, "Texture packing for font failed!");
			currRowLen = glyphSize.width;
			estimatedTextureHeight += maxLastRowHeight;
			maxLastRowHeight = glyphSize.height;
		}
	}

	// Make sure it's power of two
	size_t power = 1;
	while (power < estimatedTextureHeight)
		power <<= 1;
	estimatedTextureHeight = power;
	ASSERTE(estimatedTextureHeight > 0, "Texture packing for font failed!");

	// Create texture 2D for the face
	face.TextureProxy = gEngine->GetRenderingDevice()->CreateTexture(TEXTURE_WIDTH, estimatedTextureHeight, 1, eTextureUsageType::FONT);

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
		glyph.Size = Vector((float)glyphSize.width, (float)glyphSize.height, 0.f);
		glyph.Bearing = Vector((float)face.FTFace->glyph->bitmap_left, (float)face.FTFace->glyph->bitmap_top, 0.f);
		glyph.Advance = (float)face.FTFace->glyph->advance.x / 64.0f;

		if (face.FTFace->glyph->format != FT_GLYPH_FORMAT_BITMAP)
		{
			FT_Render_Glyph(face.FTFace->glyph, FT_RENDER_MODE_NORMAL);
		}

		face.Characters.insert(std::pair<char, FontFace::FontGlyph>(glyphSize.Glyph, glyph));
		ASSERTE(face.FTFace->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY, "Unsupported pixel mode!");
		ASSERTE(face.FTFace->glyph->bitmap.pitch >= 0 , "Negative pitch is not supported!");

		if(glyphSize.width > 0 && glyphSize.height > 0)
			face.TextureProxy->SetSubContent(glyphSize.width, glyphSize.height, xoffset, yoffset, face.FTFace->glyph->bitmap.buffer);
	}

	gConsole.LogDebug("Face of size {} for font {} loaded sucessfully! Texture size: {} x {}", height, FontPath, TEXTURE_WIDTH, estimatedTextureHeight);
}
