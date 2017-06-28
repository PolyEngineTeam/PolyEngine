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

	/// Enum used to identify components.
	/// User should make his own enum for custom components.
	/// Custom components ID's should be equal or bigger than eEngineComponents::_COUNT
	/// @see RegisterComponent()
	enum class eEngineComponents
	{
		TRANSFORM,
		BASE_CAMERA,
		MESH_RENDERING,
		FREE_FLOAT_MOVEMENT,
		SCREEN_SPACE_TEXT,
		_COUNT
	};

	/// Enum used to identify world components.
	/// User should make his own enum for custom world components.
	/// Custom world components ID's should be equal or bigger than eEngineWorldComponents::_COUNT
	/// @see RegisterWorldComponent()
	enum class eEngineWorldComponents
	{
		INPUT,
		VIEWPORT,
		TIME,
		DEBUG,
		DEFERRED_TASK,
		_COUNT
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT IGame : public BaseObject<>
	{
	public:
		virtual void RegisterEngine(Engine* engine) = 0;
		virtual void Init() = 0;
		virtual void Deinit() = 0;
	};

	/// Central part of engine.
	/// It provides elementary functions such as GetWorld() or RegisterUpdatePhase()
	/// Every update phase registered with RegisterUpdatePhase() is executed from here in Update().
	/// @see IGame
	class ENGINE_DLLEXPORT Engine : public BaseObject<>
	{
	public:
		/// Creates default world and renderer.
		/// It also causes pointers exchange between engine and game.
		/// @param game - pointer to IGame instance
		/// @see IGame::RegisterEngine()
		Engine(IGame* game, IRenderingDevice* device);

		/// Deletes world instance
		/// @see Engine()
		~Engine();

		/// Registers engine components. Registers and creates world components.
		/// Also registers engine update phases and initializes renderer object as well as game dbject. 
		/// @param context - pointer to structure with rendering context parameters.
		/// @return true when engine initialized succesfully.
		/// @see RegisterUpdatePhase()
		/// @see RegisterComponent()
		/// @see RegisterWorldComponent()
		bool Init();

		/// Deinitializes game and renderer.
		/// Also deletes Renderer.
		/// @see IGame::Deinit();
		/// @see IRenderingContext::Deinit();
		void Deinit();

		/// Enum used to specify the part of update to which a given phase is registered.
		/// @see RegisterUpdatePhase()
		enum class eUpdatePhaseOrder
		{
			PREUPDATE,
			UPDATE,
			POSTUPDATE,
			_COUNT
		};

		/// Registers a PhaseUpdateFunction to be executed in the update.
		/// part of a single frame in the same order as they were passed in.
		/// @param phaseFunction - void function(World*)
		/// @param order - enum eUpdatePhaseOrder value
		/// @see eUpdatePhaseOrder
		void RegisterUpdatePhase(const PhaseUpdateFunction& phaseFunction, eUpdatePhaseOrder order);

		/// Executes update phases functions that were registered in RegisterUpdatePhase().
		/// Functions are executrd with given order and with given update phase order.
		/// @see RegisterUpdatePhase()
		/// @see eUpdatePhaseOrder
		void Update();

		/// Pushes input event to an input queue with specified event type and key code.
		/// One of four functions handling incoming input events.
		/// @param key - key code
		/// @see KeyDown()
		/// @see UpdateMousePos()
		/// @see UpdateWheelPos()
		/// @see eKey
		void KeyDown(eKey key) { InputEventsQueue.PushBack({eInputEventType::KEYDOWN, key}); }

		/// Pushes input event to an input queue with specified event type and key code.
		/// One of four functions handling incoming input events.
		/// @param key - key code
		/// @see KeyUp()
		/// @see UpdateMousePos()
		/// @see UpdateWheelPos()
		/// @see eKey
		void KeyUp(eKey key) { InputEventsQueue.PushBack({eInputEventType::KEYUP, key}); }

		/// Pushes input event to an input queue with specified event type and key code.
		/// One of four functions handling incoming input events.
		/// @param pos - new mouse position
		/// @see KeyUp()
		/// @see KeyDown()
		/// @see UpdateWheelPos()
		void UpdateMousePos(const Vector& pos) { InputEventsQueue.PushBack({eInputEventType::MOUSEMOVE, pos}); }

		/// Pushes input event to an input queue with specified event type and key code.
		/// One of four functions handling incoming input events.
		/// @param pos - new wheel position
		/// @see KeyDown()
		/// @see KeyDown()
		/// @see UpdateMousePos()
		void UpdateWheelPos(const Vector& pos) { InputEventsQueue.PushBack({eInputEventType::WHEELMOVE, pos}); }

		/// Returns current base world refference.
		/// @return BaseWorld - current base world
		/// @see World
		World& GetWorld() { return *BaseWorld; }

		/// Registers component tyoe for further use.
		/// Registered class must inherit from ComponentBase class.
		/// @tparam T - component typename
		/// @param id - specifies what id should be associated to registered component
		/// @see eEngineComponents
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

		/// If given component is registered function returns associated ID.
		/// @tparam T - typename which ID is requested
		/// @return ID - associated ID
		/// @see RegisterComponent()
		/// @see eEngineComponents
		template<typename T> size_t GetComponentID() const
		{
			ASSERTE(ComponentTypeMap.find(typeid(T)) != ComponentTypeMap.end(), "Component was not registered!");
			return ComponentTypeMap.at(typeid(T));
		}

		/// Registers world component tyoe for further use.
		/// Registered class must inherit from ComponentBase class.
		/// @tparam T - component typename
		/// @param id - specifies what id should be associated to registered component
		/// @see eEngineWorldComponents
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

		/// If given world component is registered function returns associated ID.
		/// @tparam T - typename which ID is requested
		/// @return ID - associated ID
		/// @see RegisterWorldComponent()
		/// @see eEngineWorldComponents
		template<typename T> size_t GetWorldComponentID() const
		{
			ASSERTE(WorldComponentTypeMap.find(typeid(T)) != WorldComponentTypeMap.end(), "World component was not registered!");
			return WorldComponentTypeMap.at(typeid(T));
		}

		/// Returns pointer to renderer
		/// @return Renderer - pointer to IRenderingContext instance.
		/// @see IRenderingContext
		IRenderingDevice* GetRenderingContext() const { return Renderer; }

		/// Returns refference to input queue needed by InputPhase.
		/// @see InputSystem::InputPhase()
		InputQueue& GetInputQueue() { return InputEventsQueue; }

		/// Makes renderer resizes its context.
		/// @param size - new screen size
		/// @see IRenderingContext::Resize()
		void ResizeScreen(const ScreenSize& size);

	private:
		/// Executes all phases registered in given order.
		/// @see RegisterUpdatePhase()
		/// @see eUpdatePhaseOrder
		inline void UpdatePhases(eUpdatePhaseOrder order)
		{
			HEAVY_ASSERTE(order != eUpdatePhaseOrder::_COUNT, "_COUNT enum value passed to UpdatePhases(), which is an invalid value");
			for (auto& update : GameUpdatePhases[static_cast<int>(order)])
				update(&GetWorld());
		}

		World* BaseWorld;
		IGame* Game;
		IRenderingDevice* Renderer;
		InputQueue InputEventsQueue;

		Dynarray<PhaseUpdateFunction> GameUpdatePhases[static_cast<int>(eUpdatePhaseOrder::_COUNT)];

		std::unordered_map<std::type_index, size_t> ComponentTypeMap;
		std::unordered_map<std::type_index, size_t> WorldComponentTypeMap;
	};

	ENGINE_DLLEXPORT extern Engine* gEngine;
}

#define DECLARE_GAME() extern "C" { DEVICE_DLLEXPORT Poly::IGame* __stdcall CreateGame(); }
#define DEFINE_GAME(type) Poly::IGame* CreateGame() { return new type(); }