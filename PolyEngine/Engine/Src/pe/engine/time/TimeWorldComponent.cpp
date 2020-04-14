#include <pe/engine/EnginePCH.hpp>

#include <pe/api/ecs/Entity.hpp>
#include <pe/engine/time/TimeWorldComponent.hpp>

RTTI_DEFINE_COMPONENT(::pe::engine::time::TimeWorldComponent)

namespace pe::engine::time {

TimeWorldComponent::TimeWorldComponent()
{
	LastFrameTime = std::chrono::steady_clock::now();
	Timers[(size_t) eEngineTimer::SYSTEM] = Timer(false);
	Timers[(size_t) eEngineTimer::GAMEPLAY] = Timer(true);
}

}