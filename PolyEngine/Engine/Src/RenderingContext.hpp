#pragma once

#include <Core.hpp>

namespace Poly {

	struct ENGINE_DLLEXPORT IRenderingContextParams : public BaseObject<> {};

	class IRenderingContext : public BaseObject<> {
	public:
		virtual bool Init(const IRenderingContextParams* context) = 0;
		virtual void Deinit() = 0;

		virtual void EndFrame() = 0;
	};

	IRenderingContext* CreateRenderingContext();
}