#pragma once

#include <Core.hpp>

namespace Poly {

	struct ENGINE_DLLEXPORT IRenderingContext : public BaseObject<> {};

	class IRenderer : public BaseObject<> {
	public:
		virtual bool Init(const IRenderingContext* context) = 0;
		virtual void Deinit() = 0;
		virtual void Update(float dt) = 0;

		// Immediate
		virtual void DrawPoint(const Vector& pos, const Color& color) = 0;
		virtual void DrawLine(const Vector& pos1, const Vector& pos2, const Color& color1, const Color& color2) = 0;
		virtual void DrawTraingle(const Vector& pos1, const Vector& pos2, const Vector& pos3, const Color& color1, const Color& color2, const Color& color3) = 0;

		// Imediate helpers
		inline void DrawLine(const Vector& pos1, const Vector& pos2, const Color& color) { DrawLine(pos1, pos2, color, color); }
		inline void DrawTraingle(const Vector& pos1, const Vector& pos2, const Vector& pos3, const Color& color) { DrawTraingle(pos1, pos2, pos3, color, color, color); }
	};

	IRenderer* CreateRenderer();
}