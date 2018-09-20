#include <EnginePCH.hpp>

#include <UI/Text2D.hpp>
#include <Resources/FontResource.hpp>
#include <Resources/ResourceManager.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

Text2D::~Text2D()
{
	if(Font)
		ResourceManager<FontResource>::Release(Font);
}

void Text2D::SetFont(const String& fontName, eResourceSource source)
{
	FontName = fontName;
	ResSource = source;
	Font = ResourceManager<FontResource>::Load(fontName, source);
}

void Text2D::UpdateDeviceBuffers() const
{
	if (!Dirty)
		return;
	Dirty = false;

	// Create buffers on GPU
	if (TextFieldBufferProxy == nullptr)
	{
		TextFieldBufferProxy = gEngine->GetRenderingDevice()->CreateTextFieldBuffer();
	}

	if(!Font)
		Font = ResourceManager<FontResource>::Load(FontName, ResSource);

	const FontResource::FontFace& face = Font->GetFace(FontSize);

	// Iterate through all characters
	float x = 0;
	float y = 0;
	float scale = 1;
	Dynarray<ITextFieldBufferDeviceProxy::TextFieldLetter> letters(Text.GetLength());
	for (size_t i = 0; i < Text.GetLength(); ++i)
	{
		ITextFieldBufferDeviceProxy::TextFieldLetter letter;

		char c = Text[i];
		auto it = face.Characters.find(c);
		FontResource::FontFace::FontGlyph ch = it->second;

		letter.PosX = x + ch.Bearing.X * scale;
		letter.PosY = y - (ch.Size.Y - ch.Bearing.Y) * scale;

		letter.SizeX = ch.Size.X * scale;
		letter.SizeY = ch.Size.Y * scale;

		letter.MinU = ch.TextureUV[0].X;
		letter.MaxU = ch.TextureUV[1].X;

		letter.MinV = ch.TextureUV[0].Y;
		letter.MaxV = ch.TextureUV[1].Y;

		if(letter.SizeX > 0&& letter.SizeY > 0)
			letters.PushBack(letter);

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += ch.Advance * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}

	if(letters.GetSize() > 0)
		TextFieldBufferProxy->SetContent(letters.GetSize(), letters.GetData());
}

const ITextureDeviceProxy* Poly::Text2D::GetFontTextureProxy() const
{
	return Font->GetFace(FontSize).TextureProxy.get();
}