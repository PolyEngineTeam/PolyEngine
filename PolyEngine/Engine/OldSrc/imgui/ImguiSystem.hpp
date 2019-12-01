#pragma once

#include <pe/Defines.hpp>
#include <pe/api/ecs/ISystem.hpp>

namespace pe::engine::imgui
{
	class Scene;

	class ImguiSystem : public ISystem
	{
	public:
		ImguiSystem();

		void OnUpdate(Scene* scene) override;
	};
}