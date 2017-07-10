#pragma once

#include <functional>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <memory>

#include <Core.hpp>
#include "IRenderingDevice.hpp"

#include "InputSystem.hpp"

namespace Poly
{
	class World;
	class Engine;
	typedef std::function<void(World*)> PhaseUpdateFunction;

	/// <summary>Enum used to identify components.
	/// User should make his own enum for custom components.
	/// Custom components ID's should be equal to or bigger than eEngineComponents::_COUNT. </summary>
	/// <see cref="RegisterComponent()">
	enum class eEngineComponents
	{
		TRANSFORM,
		BASE_CAMERA,
		MESH_RENDERING,
		FREE_FLOAT_MOVEMENT,
		SCREEN_SPACE_TEXT,
		_COUNT
	};

	/// <summary>Enum used to identify world components.
	/// User should make his own enum for custom world components.
	/// Custom world components ID's should be equal or bigger than eEngineWorldComponents::_COUNT.</summary>
	/// <see cref="RegisterWorldComponent()">
	enum class eEngineWorldComponents
	{
		INPUT,
		VIEWPORT,
		TIME,
		DEBUG,
		DEFERRED_TASK,
		_COUNT
	};

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
		/// <summary>Constructs engine instance. Registers engine components. Registers and creates world components.
		/// Registers engine update phases and initializes game dbject. </summary>
		/// <param name="game">Pointer to IGame instance.</param>
		/// <param name="device">Pointer to IRenderingDevice instance.</param>
		Engine(std::unique_ptr<IGame> game, std::unique_ptr<IRenderingDevice> device);

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

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of four functions handling incoming input events.</summary>
		/// <param name="pos">New mouse position.</param>
		void UpdateMousePos(const Vector& pos) { InputEventsQueue.PushBack({eInputEventType::MOUSEMOVE, pos}); }

		/// <summary>Pushes input event to an input queue with specified event type and key code.
		/// One of four functions handling incoming input events.</summary>
		/// <param name="pos">New wheel position.</param>
		void UpdateWheelPos(const Vector& pos) { InputEventsQueue.PushBack({eInputEventType::WHEELMOVE, pos}); }

		/// <summary>Returns current base world refference.</summary>
		/// <returns>Pointer to current world.</returns>
		World* GetWorld() { return BaseWorld.get(); }

		/// <summary>Registers component tyoe for further use.
		/// Registered class must inherit from ComponentBase class.</summary>
		/// <tparam name="T">component typen</tparam>
		/// <param name="id">Specifies what id should be associated to registered component.</param>
		template<typename T> void RegisterComponent(size_t id)
		{
			ASSERTE(
				([this, &id] () ->bool
				{
					for(auto it = ComponentTypeMap.begin(); it != ComponentTypeMap.end(); ++it)
						if(it->first == typeid(T) || it->second == id) return false;
					return true;
				})(),
				"Component type or id was registered twice!");

			ComponentTypeMap[typeid(T)] = id;
		}

		/// <summary>If given component is registered function returns associated ID.</summary>
		/// <tparam name="T">Typename which ID is requested</tparam>
		/// <returns>Associated ID.</returns>
		template<typename T> size_t GetComponentID() const
		{
			ASSERTE(ComponentTypeMap.find(typeid(T)) != ComponentTypeMap.end(), "Component was not registered!");
			return ComponentTypeMap.at(typeid(T));
		}

		/// <summary>Registers world component tyoe for further use.
		/// Registered class must inherit from ComponentBase class.</summary>
		/// <tparam name="T">Component type</tparam>
		/// <param name="id">Specifies what id should be associated to registered component.</param>
		template<typename T> void RegisterWorldComponent(size_t id)
		{
			ASSERTE(
				([this, &id] () ->bool
				{
					for(auto it = WorldComponentTypeMap.begin(); it != WorldComponentTypeMap.end(); ++it)
						if(it->first == typeid(T) || it->second == id) return false;
					return true;
				})(),
				"World component type or id was registered twice!");

			WorldComponentTypeMap[typeid(T)] = id;
		}

		/// <summary>If given world component is registered function returns associated ID.</summary>
		/// <tparam name="T">Typename which ID is requested.</tparam>
		/// <returns>Associated ID.</returns>
		template<typename T> size_t GetWorldComponentID() const
		{
			ASSERTE(WorldComponentTypeMap.find(typeid(T)) != WorldComponentTypeMap.end(), "World component was not registered!");
			return WorldComponentTypeMap.at(typeid(T));
		}

		/// <summary>Returns pointer to rendering device.</summary>
		/// <returns>Pointer to IRenderingDevice instance.</returns>
		IRenderingDevice* GetRenderingDevice() const { return RenderingDevice.get(); }

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
		InputQueue InputEventsQueue;

		Dynarray<PhaseUpdateFunction> GameUpdatePhases[static_cast<int>(eUpdatePhaseOrder::_COUNT)];

		std::unordered_map<std::type_index, size_t> ComponentTypeMap;
		std::unordered_map<std::type_index, size_t> WorldComponentTypeMap;
	};

	ENGINE_DLLEXPORT extern Engine* gEngine;
}

#define DECLARE_GAME() extern "C" { DEVICE_DLLEXPORT Poly::IGame* POLY_STD_CALL CreateGame(); }
#define DEFINE_GAME(type) Poly::IGame* CreateGame() { return new type(); }
