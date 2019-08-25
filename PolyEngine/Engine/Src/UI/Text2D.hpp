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
		Text2D(const core::storage::String& fontName, eResourceSource source, size_t fontSize, const core::storage::String& text = "", const core::math::Color& fontColor = core::math::Color(1,1,1))
			: Text(text), FontName(fontName), ResSource(source), FontSize(fontSize), FontColor(fontColor) {}
		~Text2D() override;

		void SetText(const core::storage::String& text) { Text = text; Dirty = true; }
		void SetFont(const core::storage::String& fontName, eResourceSource source);
		void SetFontSize(size_t size) { FontSize = size; Dirty = true; }
		void SetFontColor(const core::math::Color& fontColor) { FontColor = fontColor; }

		const core::storage::String& GetText() const { return Text; }
		const core::storage::String& GetFont() const { return FontName; }
		const size_t& GetFontSize() const { return FontSize; }
		const core::math::Color& GetFontColor() const { return FontColor;  }


		void UpdateDeviceBuffers() const;

		const ITextFieldBufferDeviceProxy* GetTextFieldBuffer() const { return TextFieldBufferProxy.get(); }
		const ITextureDeviceProxy* GetFontTextureProxy() const;
	private:

		core::storage::String Text;
		core::storage::String FontName;
		eResourceSource ResSource;
		size_t FontSize = 0;
		core::math::Color FontColor;

		mutable bool Dirty = true;

		mutable std::unique_ptr<ITextFieldBufferDeviceProxy> TextFieldBufferProxy;
		mutable FontResource* Font = nullptr;
	};
}