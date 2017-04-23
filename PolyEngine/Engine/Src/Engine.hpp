#pragma once

#include <functional>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <memory>

#include <Core.hpp>
#include "RenderingContext.hpp"
#include "InputQueue.hpp"

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
		_COUNT
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT IGame : public BaseObject<> 
	{
	public:
		virtual void RegisterEngine(Engine* engine) = 0;
		virtual void Init() = 0;
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

		void Update(float dt);

		void KeyDown(eKey key) { InputEventsQueue.Push({eEventType::KEYDOWN, key}); }
		void KeyUp(eKey key) { InputEventsQueue.Push({eEventType::KEYUP, key}); }
		void UpdateMousePos(const Vector& pos) { InputEventsQueue.Push({eEventType::MOUSEMOVE, pos}); }
		void UpdateWheelPos(const Vector& pos) { InputEventsQueue.Push({eEventType::WHEELMOVE, pos}); }

		World& GetWorld() { return *BaseWorld; }

		//------------------------------------------------------------------------------
		template<typename T> void RegisterComponent(size_t id) 
		{ 
			ASSERTE(ComponentTypeMap.find(typeid(T)) == ComponentTypeMap.end(), "Component type was requstered twice!");
			ComponentTypeMap[typeid(T)] = id;
		}

		//------------------------------------------------------------------------------
		template<typename T> size_t GetComponentID() const 
		{
			ASSERTE(ComponentTypeMap.find(typeid(T)) != ComponentTypeMap.end(), "Component type was not requstered!");
			return ComponentTypeMap.at(typeid(T));
		}
	private:
		//------------------------------------------------------------------------------
		inline void UpdatePhases(eUpdatePhaseOrder order)
		{
			HEAVY_ASSERTE(order != eUpdatePhaseOrder::_COUNT, "Count enum value passed to UpdatePhases(), which is an invalid value");
			for (auto& update : GameUpdatePhases[static_cast<int>(order)])
				update(&GetWorld());
		}

		World* BaseWorld;
		IGame* Game;
		IRenderingContext* Renderer;

		Dynarray< PhaseUpdateFunction > GameUpdatePhases[static_cast<int>(eUpdatePhaseOrder::_COUNT)];

		InputQueue InputEventsQueue;

		std::unordered_map<std::type_index, size_t> ComponentTypeMap;
	};
}

#define REGISTER_COMPONENT(engine, type, id) engine->RegisterComponent<type>((size_t)id)
#define GET_COMPONENT_ID(engine, type) engine->GetComponentID<type>()