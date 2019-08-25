#pragma once

#include <pe/Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Rendering/RenderingSystem.hpp>
#include <UI/Text2D.hpp>

namespace Poly {

	// This is temporary class
	class ENGINE_DLLEXPORT ScreenSpaceTextComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::ScreenSpaceTextComponent) { NO_RTTI_PROPERTY(); }

		ScreenSpaceTextComponent(const core::math::Vector2i& pos, const core::storage::String& fontName, eResourceSource source, size_t fontSize, const core::storage::String& text = "", const core::math::Color& fontColor = core::math::Color(1, 1, 1))
			: Text(fontName, source, fontSize, text, fontColor), ScreenPosition(pos) {}

		Text2D& GetText() { return Text; }
		const Text2D& GetText() const { return Text; }

		void SetText(const core::storage::String& text) { Text.SetText(text); }

		void SetScreenPosition(const core::math::Vector2i& pos) { ScreenPosition = pos; }
		const core::math::Vector2i& GetScreenPosition() const { return ScreenPosition; }
	private:
		Text2D Text;
		core::math::Vector2i ScreenPosition;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ScreenSpaceTextComponent)
}