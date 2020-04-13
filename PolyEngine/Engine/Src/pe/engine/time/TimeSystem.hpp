#pragma once

#include <pe/Defines.hpp>
#include <pe/api/ecs/ISystem.hpp>

namespace pe::engine::time
{
	enum class eEngineTimer { SYSTEM, GAMEPLAY, _COUNT };

	class ENGINE_DLLEXPORT TimeSystem : public ::pe::api::ecs::ISystem
	{
	public:
		void onUpdate(::pe::api::ecs::Scene* scene) override;

		static void  RegisterTimer(::pe::api::ecs::Scene* scene, size_t id, bool isPausable = false, double multiplier = 1);

		static double GetTimerDeltaTime(::pe::api::ecs::Scene* scene, size_t timerId);
		static double GetTimerDeltaTime(::pe::api::ecs::Scene* scene, eEngineTimer timerType);

		static double GetTimerElapsedTime(::pe::api::ecs::Scene* scene, size_t timerId);
		static double GetTimerElapsedTime(::pe::api::ecs::Scene* scene, eEngineTimer timerType);

		static double GetTimerMultiplier(::pe::api::ecs::Scene* scene, size_t timerId);
		static double GetTimerMultiplier(::pe::api::ecs::Scene* scene, eEngineTimer timerType);
	};
}