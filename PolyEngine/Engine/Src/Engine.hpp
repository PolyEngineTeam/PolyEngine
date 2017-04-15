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

		void KeyDown(const eKey& key) { InputEventsQueue.Push({eEventType::KEYDOWN, key}); }
		void KeyUp(const eKey& key) { InputEventsQueue.Push({eEventType::KEYUP, key}); }
		void UpdateMousePos(const Vector& pos) { InputEventsQueue.Push({eEventType::MOUSEMOVE, pos}); }
		void UpdateWheelPos(const Vector& pos) { InputEventsQueue.Push({eEventType::WHEELMOVE, pos}); }

		World& GetWorld() { return BaseWorld; }
	private:
		World BaseWorld;
		IGame* Game;
		IRenderingContext* Renderer;

		InputQueue InputEventsQueue;
	};
}