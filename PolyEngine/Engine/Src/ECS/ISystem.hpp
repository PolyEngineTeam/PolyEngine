#pragma once

#include <pe/Defines.hpp>

namespace Poly
{
	class Scene;
	class ENGINE_DLLEXPORT ISystem : public ::pe::core::BaseObject<>
	{
	public:
		virtual void OnUpdate(Scene* scene) = 0;
	};
}