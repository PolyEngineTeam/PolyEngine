#pragma once

#include <pe/Defines.hpp>

#include <Rendering/IRenderingDevice.hpp>
#include <Audio/OpenALDevice.hpp>
#include <Input/InputSystem.hpp>
#include <ECS/ISystem.hpp>
#include <pe/core/utils/EnumUtils.hpp>
#include "ECS/ComponentIDGenerator.hpp"
#include "Editor/IEditor.hpp"

#include <pe/api/IEngine.hpp>

namespace Poly
{
	class Scene;
	class Engine;
	typedef std::function<void(Scene*)> PhaseUpdateFunction;

	/// <summary>Abstract class that every game has to inherit from.</summary>
	class ENGINE_DLLEXPORT IGame : public pe::api::IEngine
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
		Engine(bool testRun = false, IEditor* editor = nullptr);

		/// <summary>Deletes engine instance.</summary>
		~Engine();

		/// <summary>Enum used to specify the part of update to which a given phase is registered.</summary>
		/// <see cref="Engine.RegisterUpdatePhase()"/>
		enum class eUpdatePhaseOrder
		{
			PREUPDATE,
			EDITOR,
			UPDATE, //TODO(squares): rename this to GAME?
			POSTUPDATE,
			_COUNT
		};

		/// <summary>Registers engine components. Registers and creates world components.
		/// Registers engine update phases and initializes game dbject. </summary>
		/// <param name="game">Pointer to IGame instance.</param>
		/// <param name="device">Pointer to IRenderingDevice instance.</param>
		void Init(std::unique_ptr<IGame> game, std::unique_ptr<IRenderingDevice> device);

		void StartGame();

		void EndGame();

		/// <summary>Registers a PhaseUpdateFunction to be executed in the update.</summary>
		/// <param name="phaseFunction"/>
		void RegisterGameUpdatePhase(const PhaseUpdateFunction& phaseFunction) { RegisterUpdatePhase(phaseFunction, eUpdatePhaseOrder::UPDATE); }
		void RegisterEditorUpdatePhase(const PhaseUpdateFunction& phaseFunction) { RegisterUpdatePhase(phaseFunction, eUpdatePhaseOrder::EDITOR); }

		/// <summary>Registers a System's PhaseUpdateFunction to be executed in the update.</summary>
		/// <param name="system"/>
		void RegisterGameUpdatePhase(std::unique_ptr<ISystem> system) { RegisterSystem(std::move(system), eUpdatePhaseOrder::UPDATE); }

		/// <summary>Executes update phases functions that were registered in RegisterUpdatePhase().
		/// Functions are executrd with given order and with given update phase order.</summary>
		/// <see cref="Engine.RegisterUpdatePhase()"/>
		/// <see cref="Engine.eUpdatePhaseOrder"/>
		void Update();

		void Update(std::vector<eUpdatePhaseOrder> phasesUpdate);

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of eight functions handling incoming input events.</summary>
		/// <param name="key">Key code</param>
		void KeyDown(eKey key) { InputEventsQueue.PushBack({eInputEventType::KEYDOWN, key}); }

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of eight functions handling incoming input events.</summary>
		/// <param name="key">Key code</param>
		void KeyUp(eKey key) { InputEventsQueue.PushBack({eInputEventType::KEYUP, key}); }

		/// <summary>Pushes input event to an input queue with specified UTF8 character.
		/// One of eight functions handling incoming input events.</summary>
		/// <param name="key">Unicode UTF8 character</param>
		void AddCharacterUTF8(const char* charUTF8) { InputEventsQueue.PushBack({eInputEventType::TEXTCHAR, charUTF8}); }

		/// <summary>Pushes input event to an input queue with specified event type and button code.
		/// One of eight functions handling incoming input events.</summary>
		/// <param name="button">Mouse button code</param>
		void MouseButtonDown(eMouseButton button) { InputEventsQueue.PushBack({eInputEventType::MOUSEBUTTONDOWN, button}); }

		/// <summary>Pushes input event to an input queue with specified event type and button code.
		/// One of eight functions handling incoming input events.</summary>
		/// <param name="button">Mouse button code</param>
		void MouseButtonUp(eMouseButton button) { InputEventsQueue.PushBack({eInputEventType::MOUSEBUTTONUP, button}); }

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of eight functions handling incoming input events.</summary>
		/// <param name="pos">Mouse delta pos.</param>
		void UpdateMouseMove(const Vector2i& delta) { InputEventsQueue.PushBack({eInputEventType::MOUSEMOVE, delta }); }

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of eight functions handling incoming input events.</summary>
		/// <param name="pos">New mouse position.</param>
		void UpdateMousePos(const Vector2i& pos) { InputEventsQueue.PushBack({ eInputEventType::MOUSEPOS, pos }); }

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of eight functions handling incoming input events.</summary>
		/// <param name="pos">Wheel delta position.</param>
		void UpdateWheelPos(const Vector2i& deltaPos) { InputEventsQueue.PushBack({eInputEventType::WHEELMOVE, deltaPos}); }

        /// <summary>Pushes adding input controller event to an input queue with specified SDL_GameController id</summary>
        /// <param name="id">SDL Joystick Instance id</param>
        void AddController(size_t id) { InputEventsQueue.PushBack({eInputEventType::CONTROLLER_ADDED, id}); };

        /// <summary>Pushes removing input controller event to an input queue with specified SDL_Joystick instance id</summary>
        /// <param name="id">SDL Joystick instance id</param>
        void RemoveController(size_t id) { InputEventsQueue.PushBack({eInputEventType::CONTROLLER_REMOVED, id}); };

        /// <summary>Pushes controller button down event to an input queue</summary>
        /// <param name="id">SDL Joystick instance id</param>
        /// <param name="button">Button pressed enum instance</param>
        void ControllerButtonDown(size_t id, eControllerButton button) { InputEventsQueue.PushBack({eInputEventType::CONTROLLER_BUTTON_DOWN, id, button}); };

        /// <summary>Pushes controller button up event to an input queue</summary>
        /// <param name="id">SDL Joystick instance id</param>
        /// <param name="button">Button released enum instance</param>
        void ControllerButtonUp(size_t id, eControllerButton button) { InputEventsQueue.PushBack({eInputEventType::CONTROLLER_BUTTON_UP, id, button}); };

        /// <summary>Pushes controller axis motion event to an input queue</summary>
        /// <param name="id">SDL Joystick instance id</param>
        /// <param name="axis">Axis enum instance</param>
        /// <param name="value">New axis position (SDL raw, not normalized)</param>
        void ControllerAxisMotion(size_t id, eControllerAxis axis, i16 value) { InputEventsQueue.PushBack({eInputEventType::CONTROLLER_AXIS_MOTION, id, axis, value/35768.0f}); };

		///functions for closing the game
		bool IsQuitRequested() const;
		void RequestGameQuit();
		
		bool ShouldCaptureMouse() const { return MouseCaptureEnabled; }
		void SetMouseCapture(bool enabled) { MouseCaptureEnabled = enabled; }

		bool ShouldMouseBeVisible() const { return MouseVisible; }
		void SetMouseVisible(bool visible) { MouseVisible = visible; }

		/// <summary>Returns current base world refference.</summary>
		/// <returns>Pointer to current world.</returns>
		Scene* GetActiveScene() { return ActiveScene.get(); }
		void LoadDefaultScene();

		IEditor* GetEditor() { return Editor; }

		Scene* GetCurrentlySerializedScene() { return SerializedScene; }
		void SetCurrentlySerializedScene(Scene* s) { ASSERTE(SerializedScene == nullptr || s == nullptr, "Setting scene again!"); SerializedScene = s; }

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

		/// <summary>Returns refference to output queue needed by Main.</summary>
		/// <returns>Reference to OutputQueue instance.</returns>
		OutputQueue& GetOutputQueue() { return OutputEventsQueue; }

		/// <summary>Makes renderer resizes its context.</summary>
		/// <param name="size">New screen size</param>
		void ResizeScreen(const ScreenSize& size);

		const char* (*GetClipboardTextFunction)(void* user_data);
		
		void(*SetClipboardTextFunction)(void* user_data, const char* text);

	private:
		inline void UpdatePhases(eUpdatePhaseOrder order)
		{
			HEAVY_ASSERTE(order != eUpdatePhaseOrder::_COUNT, "_COUNT enum value passed to UpdatePhases(), which is an invalid value");
			for (auto&  update : GameUpdatePhases[order])
				update->OnUpdate(GetActiveScene());
		}

		/// @deprecated
		/// Registers a PhaseUpdateFunction to be executed in the update.
		/// part of a single frame in the same order as they were passed in.
		/// @param phaseFunction - void function(Scene*)
		/// @param order - enum eUpdatePhaseOrder value
		/// @see eUpdatePhaseOrder
		void RegisterUpdatePhase(const PhaseUpdateFunction& phaseFunction, eUpdatePhaseOrder order);

		/// Registers a ISystem containing PhaseUpdateFunction to be executed in the update.
		/// part of a single frame in the same order as they were passed in.
		/// @param system - std::unique_ptr<ISystem> containing void function(Scene*)
		/// @param order - enum eUpdatePhaseOrder value
		/// @see eUpdatePhaseOrder
		void RegisterSystem(std::unique_ptr<ISystem> system, eUpdatePhaseOrder order);

		std::unique_ptr<Scene> ActiveScene;
		Scene* SerializedScene = nullptr;
		std::unique_ptr<IGame> Game;
		std::unique_ptr<IRenderingDevice> RenderingDevice;
		IEditor* Editor = nullptr;
		OpenALDevice AudioDevice;
		InputQueue InputEventsQueue;
		OutputQueue OutputEventsQueue;

		EnumArray<std::vector<std::unique_ptr<ISystem>>, eUpdatePhaseOrder> GameUpdatePhases;

		bool QuitRequested = false; //stop the game
		bool MouseCaptureEnabled = false;
		bool MouseVisible = true;
	};

	ENGINE_DLLEXPORT extern Engine* gEngine;
}

#define DECLARE_GAME() extern "C" { GAME_DLLEXPORT Poly::IGame* POLY_STDCALL CreateGame(); }
#define DEFINE_GAME(type) Poly::IGame* POLY_STDCALL CreateGame() { return new type(); }
