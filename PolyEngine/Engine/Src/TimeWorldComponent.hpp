#pragma once

#include <Core.hpp>
#include "Timers.hpp"
#include "ComponentBase.hpp"



namespace Poly
{
	class ENGINE_DLLEXPORT TimeWorldComponent : public ComponentBase
	{
	public:	
		void Update();
		void Pause() { Time.Pause(); };
		void Unpause() { Time.Unpause(); };

		float GetDeltaTime() { return DeltaTime; };

		bool IsGamePaused() { return Time.IsPaused(); };

	private:
		PausableTimer Time;
		float DeltaTime;
	};
}