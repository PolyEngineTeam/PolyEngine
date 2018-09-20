#pragma once

#include <Defines.hpp>
#include <Rendering/IRenderingDevice.hpp>

typedef unsigned GLuint;

namespace Poly
{
	class FontResource;
	enum class eResourceSource;

	class ENGINE_DLLEXPORT Text2D : public BaseObject<>
	{
	public:
		Text2D(const String& fontName, eResourceSource source, size_t fontSize, const String& text = "", const Color& fontColor = Color(1,1,1))
			: Text(text), FontName(fontName), ResSource(source), FontSize(fontSize), FontColor(fontColor) {}
		~Text2D() override;

		void SetText(const String& text) { Text = text; Dirty = true; }
		void SetFont(const String& fontName, eResourceSource source);
		void SetFontSize(size_t size) { FontSize = size; Dirty = true; }
		void SetFontColor(const Color& fontColor) { FontColor = fontColor; }

		const String& GetText() const { return Text; }
		const String& GetFont() const { return FontName; }
		const size_t& GetFontSize() const { return FontSize; }
		const Color& GetFontColor() const { return FontColor;  }


		void UpdateDeviceBuffers() const;

		const ITextFieldBufferDeviceProxy* GetTextFieldBuffer() const { return TextFieldBufferProxy.get(); }
		const ITextureDeviceProxy* GetFontTextureProxy() const;
	private:

		String Text;
		String FontName;
		eResourceSource ResSource;
		size_t FontSize = 0;
		Color FontColor;

		mutable bool Dirty = true;

		mutable std::unique_ptr<ITextFieldBufferDeviceProxy> TextFieldBufferProxy;
		mutable FontResource* Font = nullptr;
	};
}