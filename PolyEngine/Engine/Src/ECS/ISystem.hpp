#pragma once

#include <Defines.hpp>

namespace Poly
{
	class Scene;
	class ISystem : public BaseObject<>
	{
	public:
		virtual void OnUpdate(Scene* scene) = 0;
	};
}