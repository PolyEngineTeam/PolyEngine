#pragma once

#include <pe/Defines.hpp>
#include <pe/engine/time/TimeSystem.hpp>
#include <pe/engine/time/Timer.hpp>
#include <pe/api/ecs/ComponentBase.hpp>


namespace pe::engine::time
{
	class ENGINE_DLLEXPORT TimeWorldComponent : public api::ecs::ComponentBase
	{
		friend class TimeSystem;
	public:
		RTTI_DECLARE_COMPONENT(::pe::engine::time::TimeWorldComponent) { NO_RTTI_PROPERTY(); }

		TimeWorldComponent();

		double GetSystemTime() const { return Timers.at((size_t) eEngineTimer::SYSTEM).GetTime(); };
		double GetGameplayTime() const { return Timers.at((size_t) eEngineTimer::GAMEPLAY).GetTime(); };

		bool IsPaused() const { return Paused; };

		void SetPaused(bool isPaused) { Paused = isPaused; };

	private:
		const std::chrono::milliseconds MIN_TIME_STEP_MS = std::chrono::milliseconds(60);

		std::chrono::steady_clock::time_point LastFrameTime;

		std::unordered_map<size_t, Timer> Timers;

		bool Paused = false;
	};
}

//REGISTER_COMPONENT(::pe::api::ecs::ComponentsIDGroup, ::pe::engine::time::TimeWorldComponent)