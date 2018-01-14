#pragma once

#include <functional>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <memory>

#include <Core.hpp>
#include "IRenderingDevice.hpp"
#include "OpenALDevice.hpp"

#include "InputSystem.hpp"

namespace Poly
{
	class World;
	class Engine;
	typedef std::function<void(World*)> PhaseUpdateFunction;

	/// <summary>Abstract class that every game has to inherit from.</summary>
	class ENGINE_DLLEXPORT IGame : public BaseObject<>
	{
	public:
		virtual void RegisterEngine(Engine* engine) = 0;
		virtual void Init() = 0;
		virtual void Deinit() = 0;
	};

	/// <summary>Central part of engine. It can be obtained vis gEngine global variable.</summary>
	class ENGINE_DLLEXPORT Engine : public BaseObject<>
	{
	public:
		/// <summary>Constructs engine instance.</summary>
		Engine();

		/// <summary>Deletes engine instance.</summary>
		~Engine();

		/// <summary>Enum used to specify the part of update to which a given phase is registered.</summary>
		/// <see cref="Engine.RegisterUpdatePhase()"/>
		enum class eUpdatePhaseOrder
		{
			PREUPDATE,
			UPDATE,
			POSTUPDATE,
			_COUNT
		};

		/// <summary>Registers engine components. Registers and creates world components.
		/// Registers engine update phases and initializes game dbject. </summary>
		/// <param name="game">Pointer to IGame instance.</param>
		/// <param name="device">Pointer to IRenderingDevice instance.</param>
		void Init(std::unique_ptr<IGame> game, std::unique_ptr<IRenderingDevice> device);

		/// <summary>Registers a PhaseUpdateFunction to be executed in the update.</summary>
		/// <param name="phaseFunction"/>
		void RegisterGameUpdatePhase(const PhaseUpdateFunction& phaseFunction) { RegisterUpdatePhase(phaseFunction, eUpdatePhaseOrder::UPDATE); }

		/// <summary>Executes update phases functions that were registered in RegisterUpdatePhase().
		/// Functions are executrd with given order and with given update phase order.</summary>
		/// <see cref="Engine.RegisterUpdatePhase()"/>
		/// <see cref="Engine.eUpdatePhaseOrder"/>
		void Update();

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of four functions handling incoming input events.</summary>
		/// <param name="key">Key code</param>
		void KeyDown(eKey key) { InputEventsQueue.PushBack({eInputEventType::KEYDOWN, key}); }

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of four functions handling incoming input events.</summary>
		/// <param name="key">Key code</param>
		void KeyUp(eKey key) { InputEventsQueue.PushBack({eInputEventType::KEYUP, key}); }

		/// <summary>Pushes input event to an input queue with specified event type and button code.
		/// One of four functions handling incoming input events.</summary>
		/// <param name="button">Mouse button code</param>
		void MouseButtonDown(eMouseButton button) { InputEventsQueue.PushBack({eInputEventType::MOUSEBUTTONDOWN, button}); }

		/// <summary>Pushes input event to an input queue with specified event type and button code.
		/// One of four functions handling incoming input events.</summary>
		/// <param name="button">Mouse button code</param>
		void MouseButtonUp(eMouseButton button) { InputEventsQueue.PushBack({eInputEventType::MOUSEBUTTONUP, button}); }

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of four functions handling incoming input events.</summary>
		/// <param name="pos">New mouse position.</param>
		void UpdateMousePos(const Vector2i& pos) { InputEventsQueue.PushBack({eInputEventType::MOUSEMOVE, pos}); }

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of four functions handling incoming input events.</summary>
		/// <param name="pos">Wheel delta position.</param>
		void UpdateWheelPos(const Vector2i& deltaPos) { InputEventsQueue.PushBack({eInputEventType::WHEELMOVE, deltaPos}); }

		///functions for closing the game
		bool IsQuitRequested() const;
		void RequestGameQuit();
		

		/// <summary>Returns current base world refference.</summary>
		/// <returns>Pointer to current world.</returns>
		World* GetWorld() { return BaseWorld.get(); }

		/// <summary>Returns pointer to rendering device.</summary>
		/// <returns>Pointer to IRenderingDevice instance.</returns>
		IRenderingDevice* GetRenderingDevice() const { return RenderingDevice.get(); }

		/// Returns referrence to audio renderer
		/// @return AudioRenderer - pointer to IRenderingContext instance.
		/// @see IRenderingContext
		OpenALDevice& GetAudioDevice() { return AudioDevice; }

		/// <summary>Returns refference to input queue needed by InputPhase.</summary>
		/// <returns>Reference to InputQueue instance.</returns>
		InputQueue& GetInputQueue() { return InputEventsQueue; }

		/// <summary>Makes renderer resizes its context.</summary>
		/// <param name="size">New screen size</param>
		void ResizeScreen(const ScreenSize& size);

	private:
		inline void UpdatePhases(eUpdatePhaseOrder order)
		{
			HEAVY_ASSERTE(order != eUpdatePhaseOrder::_COUNT, "_COUNT enum value passed to UpdatePhases(), which is an invalid value");
			for (auto& update : GameUpdatePhases[static_cast<int>(order)])
				update(GetWorld());
		}

		/// Registers a PhaseUpdateFunction to be executed in the update.
		/// part of a single frame in the same order as they were passed in.
		/// @param phaseFunction - void function(World*)
		/// @param order - enum eUpdatePhaseOrder value
		/// @see eUpdatePhaseOrder
		void RegisterUpdatePhase(const PhaseUpdateFunction& phaseFunction, eUpdatePhaseOrder order);

		std::unique_ptr<World> BaseWorld;
		std::unique_ptr<IGame> Game;
		std::unique_ptr<IRenderingDevice> RenderingDevice;
		OpenALDevice AudioDevice;
		InputQueue InputEventsQueue;

		Dynarray<PhaseUpdateFunction> GameUpdatePhases[static_cast<int>(eUpdatePhaseOrder::_COUNT)];

		bool QuitRequested = false; //stop the game
	};

	ENGINE_DLLEXPORT extern Engine* gEngine;
}

#define DECLARE_GAME() extern "C" { GAME_DLLEXPORT Poly::IGame* POLY_STDCALL CreateGame(); }
#define DEFINE_GAME(type) Poly::IGame* POLY_STDCALL CreateGame() { return new type(); }
