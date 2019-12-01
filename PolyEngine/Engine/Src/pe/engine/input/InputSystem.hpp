#pragma once

#include <pe/Defines.hpp>

namespace pe::api::ecs
{
	class Scene;
}

namespace pe::engine::input
{
	namespace InputSystem
	{
		void InputPhase(api::ecs::Scene*);
	}
}