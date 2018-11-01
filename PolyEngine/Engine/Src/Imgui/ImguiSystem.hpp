#pragma once

#include <Defines.hpp>
#include <ECS/ISystem.hpp>

namespace Poly
{
	class Scene;

	class ImguiSystem : public ISystem
	{
	public:
		ImguiSystem();

		void OnUpdate(Scene* scene) override;
	};
}