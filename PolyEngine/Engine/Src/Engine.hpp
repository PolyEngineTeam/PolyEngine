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
		bool Init(const IRenderingContextParams* context);
		void Deinit();

		void Update(float dt);

	private:
		IGame* Game;
		IRenderingContext* Renderer;
	};
}