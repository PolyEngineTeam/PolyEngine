#pragma once

#include <Core.hpp>
#include "RenderingContext.hpp"
#include "World.hpp"
#include "InputQueue.hpp"

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

		void KeyDown(eKey key) { InputBase.Push({eEventType::KEYDOWN, key}); }
		void KeyUp(eKey key) { InputBase.Push({eEventType::KEYUP, key}); }
		void UpdateMousePos(Vector pos) { InputBase.Push({eEventType::MOUSEMOVE, pos}); }
		void UpdateWheelPos(Vector pos) { InputBase.Push({eEventType::WHEELMOVE, pos}); }

		World& GetWorld() { return BaseWorld; }
	private:
		World BaseWorld;
		IGame* Game;
		IRenderingContext* Renderer;

		InputQueue InputBase;
	};
}