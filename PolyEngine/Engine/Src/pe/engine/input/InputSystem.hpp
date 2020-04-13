#pragma once

#include <pe/Defines.hpp>

#include <pe/api/app/App.hpp>
#include <pe/api/ecs/ISystem.hpp>
#include <pe/api/deps/DependencyPtr.hpp>

namespace pe::api::ecs
{
	class Scene;
}

namespace pe::engine::input
{
	class InputSystem : public api::ecs::ISystem
	{
		void onUpdate(api::ecs::Scene* scene) override;

	private:
		api::deps::DependencyPtr<pe::api::app::App> m_app;
	};
}