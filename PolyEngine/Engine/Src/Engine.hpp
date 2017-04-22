#pragma once

#include <functional>
#include <Core.hpp>
#include "RenderingContext.hpp"
#include "World.hpp"
#include "InputSystem.hpp"

namespace Poly 
{
	class Engine;
	typedef std::function<void(World*)> PhaseUpdateFunction;

	InputQueue* InputEventsQueue;

	class ENGINE_DLLEXPORT IGame : public BaseObject<> 
	{
	public:
		virtual void RegisterEngine(Engine* engine) = 0;
		virtual void Init() = 0;
	};

	class ENGINE_DLLEXPORT Engine : public BaseObject<> 
	{
	public:
		Engine(IGame* game);
		bool Init(const IRenderingContextParams* context);
		void Deinit();

		//////////////////////////////
		/// Enum used to specify the part of update to which a given phase is registered.
		/// @see RegisterUpdatePhase()
		//////////////////////////////
		enum class eUpdatePhaseOrder 
		{
			PREUPDATE,
			UPDATE,
			POSTUPDATE,
			_COUNT
		};

		//////////////////////////////
		/// Registers a PhaseUpdateFunction to be executed in the update
		/// part of a single frame in the same order as they were passed in.
		/// @param phaseFunction - void function(World*)
		/// @param order - enum eUpdatePhaseOrder value
		/// @see eUpdatePhaseOrder
		//////////////////////////////
		void RegisterUpdatePhase(const PhaseUpdateFunction& phaseFunction, eUpdatePhaseOrder order);

		void Update(float dt);

		void KeyDown(eKey key) { InputEventsQueue->Push({eInputEventType::KEYDOWN, key}); }
		void KeyUp(eKey key) { InputEventsQueue->Push({eInputEventType::KEYUP, key}); }
		void UpdateMousePos(const Vector& pos) { InputEventsQueue->Push({eInputEventType::MOUSEMOVE, pos}); }
		void UpdateWheelPos(const Vector& pos) { InputEventsQueue->Push({eInputEventType::WHEELMOVE, pos}); }

		World& GetWorld() { return BaseWorld; }
	private:
		inline void UpdatePhases(eUpdatePhaseOrder order)
		{
			HEAVY_ASSERTE(order != eUpdatePhaseOrder::_COUNT, "Count enum value passed to UpdatePhases(), which is an invalid value");
			for (auto& update : GameUpdatePhases[static_cast<int>(order)])
				update(&GetWorld());
		}

		World BaseWorld;
		IGame* Game;
		IRenderingContext* Renderer;

		Dynarray< PhaseUpdateFunction > GameUpdatePhases[static_cast<int>(eUpdatePhaseOrder::_COUNT)];

		InputQueue InputEventsQueue;
	};
}