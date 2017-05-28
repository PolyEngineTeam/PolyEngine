#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"

#include "Text2D.hpp"

namespace Poly {

	// This is temporary class
	class ENGINE_DLLEXPORT ScreenSpaceTextComponent : public ComponentBase
	{
		friend void RenderingSystem::RenderingPhase(World*);
	public:
		ScreenSpaceTextComponent(const Vector& pos, const String& fontName, size_t fontSize, const String& text = "", const Color& fontColor = Color(1, 1, 1))
			: Text(fontName, fontSize, text, fontColor), ScreenPosition(pos) {}

		Text2D& GetText() { return Text; }
		const Text2D& GetText() const { return Text; }

		void SetScreenPosition(const Vector& pos) { ScreenPosition = pos; }
	private:
		Text2D Text;
		Vector ScreenPosition;
	};
}