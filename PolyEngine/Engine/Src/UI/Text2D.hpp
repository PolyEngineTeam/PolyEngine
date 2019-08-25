#pragma once

#include <pe/Defines.hpp>
#include <Rendering/IRenderingDevice.hpp>

typedef unsigned GLuint;

namespace Poly
{
	class FontResource;
	enum class eResourceSource;

	class ENGINE_DLLEXPORT Text2D : public ::pe::core::BaseObject<>
	{
	public:
		Text2D(const ::pe::core::storage::String& fontName, eResourceSource source, size_t fontSize, const ::pe::core::storage::String& text = "", const ::pe::core::math::Color& fontColor = ::pe::core::math::Color(1,1,1))
			: Text(text), FontName(fontName), ResSource(source), FontSize(fontSize), FontColor(fontColor) {}
		~Text2D() override;

		void SetText(const ::pe::core::storage::String& text) { Text = text; Dirty = true; }
		void SetFont(const ::pe::core::storage::String& fontName, eResourceSource source);
		void SetFontSize(size_t size) { FontSize = size; Dirty = true; }
		void SetFontColor(const ::pe::core::math::Color& fontColor) { FontColor = fontColor; }

		const ::pe::core::storage::String& GetText() const { return Text; }
		const ::pe::core::storage::String& GetFont() const { return FontName; }
		const size_t& GetFontSize() const { return FontSize; }
		const ::pe::core::math::Color& GetFontColor() const { return FontColor;  }


		void UpdateDeviceBuffers() const;

		const ITextFieldBufferDeviceProxy* GetTextFieldBuffer() const { return TextFieldBufferProxy.get(); }
		const ITextureDeviceProxy* GetFontTextureProxy() const;
	private:

		::pe::core::storage::String Text;
		::pe::core::storage::String FontName;
		eResourceSource ResSource;
		size_t FontSize = 0;
		::pe::core::math::Color FontColor;

		mutable bool Dirty = true;

		mutable std::unique_ptr<ITextFieldBufferDeviceProxy> TextFieldBufferProxy;
		mutable FontResource* Font = nullptr;
	};
}