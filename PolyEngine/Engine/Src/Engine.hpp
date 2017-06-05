#pragma once

#include <functional>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <memory>

#include <Core.hpp>
#include "RenderingContext.hpp"

#include "InputSystem.hpp"

namespace Poly
{
	class World;
	class Engine;
	typedef std::function<void(World*)> PhaseUpdateFunction;

	//------------------------------------------------------------------------------
	enum class eEngineComponents
	{
		TRANSFORM,
		BASE_CAMERA,
		MESH_RENDERING,
		FREE_FLOAT_MOVEMENT,
		SCREEN_SPACE_TEXT,
		_COUNT
	};

	//------------------------------------------------------------------------------
	enum class eEngineWorldComponents
	{
		INPUT,
		VIEWPORT,
		TIME,
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

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT Engine : public BaseObject<>
	{
	public:
		Engine(IGame* game);
		~Engine();
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

		void Update();

		void KeyDown(eKey key) { InputEventsQueue.PushBack({eInputEventType::KEYDOWN, key}); }
		void KeyUp(eKey key) { InputEventsQueue.PushBack({eInputEventType::KEYUP, key}); }
		void UpdateMousePos(const Vector& pos) { InputEventsQueue.PushBack({eInputEventType::MOUSEMOVE, pos}); }
		void UpdateWheelPos(const Vector& pos) { InputEventsQueue.PushBack({eInputEventType::WHEELMOVE, pos}); }

		World& GetWorld() { return *BaseWorld; }

		//------------------------------------------------------------------------------
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
		//------------------------------------------------------------------------------
		template<typename T> size_t GetComponentID() const
		{
			ASSERTE(ComponentTypeMap.find(typeid(T)) != ComponentTypeMap.end(), "Component was not registered!");
			return ComponentTypeMap.at(typeid(T));
		}
		//------------------------------------------------------------------------------
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
		//------------------------------------------------------------------------------
		template<typename T> size_t GetWorldComponentID() const
		{
			ASSERTE(WorldComponentTypeMap.find(typeid(T)) != WorldComponentTypeMap.end(), "World component was not registered!");
			return WorldComponentTypeMap.at(typeid(T));
		}

		IRenderingContext* GetRenderingContext() const { return Renderer; }
		InputQueue& GetInputQueue() { return InputEventsQueue; }

		void ResizeScreen(const ScreenSize& size);
	private:
		//------------------------------------------------------------------------------
		inline void UpdatePhases(eUpdatePhaseOrder order)
		{
			HEAVY_ASSERTE(order != eUpdatePhaseOrder::_COUNT, "_COUNT enum value passed to UpdatePhases(), which is an invalid value");
			for (auto& update : GameUpdatePhases[static_cast<int>(order)])
				update(&GetWorld());
		}

		World* BaseWorld;
		IGame* Game;
		IRenderingContext* Renderer;
		InputQueue InputEventsQueue;

		Dynarray< PhaseUpdateFunction > GameUpdatePhases[static_cast<int>(eUpdatePhaseOrder::_COUNT)];

		std::unordered_map<std::type_index, size_t> ComponentTypeMap;
		std::unordered_map<std::type_index, size_t> WorldComponentTypeMap;
	};
}
