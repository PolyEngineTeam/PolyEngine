#pragma once

#include <Core.hpp>
#include "Renderer.hpp"

namespace Poly {
	class Engine;

	class ENGINE_DLLEXPORT IGame : public BaseObject<> {
	public:
		virtual void RegisterEngine(Engine* engine) = 0;
		virtual void Init() = 0;
		virtual void Update(float dt) = 0;
	};

	class ENGINE_DLLEXPORT Engine : public BaseObject<> {
	public:
		Engine(IGame* game);
		void Init(const IRenderingContext* context);
		void Deinit();

		void Update(float dt);

	private:
		IGame* Game;
		IRenderer* Renderer;
	};
}