#pragma once

#include <pe/Defines.hpp>

namespace Poly
{
	class Scene;
}

namespace EditorCameraMovementSystem
{
	void Update(Poly::Scene* scene);

	core::math::Vector GetLocalForward(const Poly::EntityTransform& transform);
	core::math::Vector GetLocalRight(const Poly::EntityTransform& transform);
	core::math::Vector GetLocalUp(const Poly::EntityTransform& transform);

	core::math::Vector GetGlobalForward(const Poly::EntityTransform& transform);
	core::math::Vector GetGlobalRight(const Poly::EntityTransform& transform);
	core::math::Vector GetGlobalUp(const Poly::EntityTransform& transform);
}