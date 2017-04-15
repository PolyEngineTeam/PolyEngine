#pragma once

#include <functional>
#include <Core.hpp>
#include "RenderingContext.hpp"
#include "World.hpp"
#include "SystemBase.hpp"

namespace Poly 
{
	class Engine;
	typedef std::function<void(World*)> PhaseUpdateFunction;

	class ENGINE_DLLEXPORT IGame : public BaseObject<> 
	{
	public:
		virtual void RegisterEngine(Engine* engine) = 0;
		virtual void Init() = 0;
		virtual void Update(float dt) = 0;
	};

	class ENGINE_DLLEXPORT Engine : public BaseObject<> 
	{
	public:
		Engine(IGame* game);
		bool Init(const IRenderingContextParams* context);
		void Deinit();

		//////////////////////////////
		/// Registers a PhaseUpdateFunction to be executed in the Preupdate 
		/// part of a single frame in the same order as they were passed in.
		/// @see RegisterUpdatePhase()
		/// @see RegisterPostupdatePhase()
		/// @param phaseFunction - void function(World*)
		//////////////////////////////
		void RegisterPreupdatePhase(PhaseUpdateFunction phaseFunction);

		//////////////////////////////
		/// Registers a PhaseUpdateFunction to be executed in the main update 
		/// part of a single frame in the same order as they were passed in.
		/// @see RegisterPreupdatePhase()
		/// @see RegisterPostupdatePhase()
		/// @param phaseFunction - void function(World*)
		//////////////////////////////
		void RegisterUpdatePhase(PhaseUpdateFunction phaseFunction);

		//////////////////////////////
		/// Registers a PhaseUpdateFunction to be executed in the Postupdate 
		/// part of a single frame in the same order as they were passed in.
		/// @see RegisterPreupdatePhase()
		/// @see RegisterUpdatePhase()
		/// @param phaseFunction - void function(World*)
		//////////////////////////////
		void RegisterPostupdatePhase(PhaseUpdateFunction phaseFunction);


		void Update(float dt);

		World& GetWorld() { return BaseWorld; }
	private:
		inline void PreupdatePhases();
		inline void UpdatePhases();
		inline void PostupdatePhases();

		World BaseWorld;
		IGame* Game;
		IRenderingContext* Renderer;
		Dynarray< PhaseUpdateFunction > GamePreupdatePhases;
		Dynarray< PhaseUpdateFunction > GameUpdatePhases;
		Dynarray< PhaseUpdateFunction > GamePostupdatePhases;
	};
}