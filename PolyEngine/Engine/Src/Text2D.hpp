#pragma once

#include <Core.hpp>

typedef unsigned GLuint;

namespace Poly
{
	class FontResource;

	class Text2D : public BaseObject<>
	{
	public:
		Text2D(const String& fontName, size_t fontSize, const String& text = "", const Color& fontColor = Color(1,1,1)) 
			: Text(text), FontName(fontName), FontSize(fontSize), FontColor(fontColor) {}
		~Text2D() override;

		void SetText(const String& text) { Text = text; Dirty = true; }
		void SetFont(const String& fontName);
		void SetFontSize(size_t size) { FontSize = size; Dirty = true; }
		void SetFontColor(const Color& fontColor) { FontColor = fontColor; }

		const String& GetText() const { return Text; }
		const String& GetFont() const { return FontName; }
		const size_t& GetFontSize() const { return FontSize; }
		const Color& GetfontColor() const { return FontColor;  }

		GLuint GetVAO() const { return VAO; }
		GLuint GetVBO() const { return VBO; }
	private:
		void UpdateDeviceBuffers();

		String Text;
		String FontName;
		size_t FontSize = 0;
		Color FontColor;

		bool Dirty = true;
		GLuint VAO = 0, VBO = 0;
		FontResource* Font = nullptr;
	};
}