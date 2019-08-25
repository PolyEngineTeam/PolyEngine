#pragma once

#include <pe/Defines.hpp>

namespace Poly
{
	class Scene;
}

namespace EditorCameraMovementSystem
{
	void Update(Poly::Scene* scene);

	::pe::core::math::Vector GetLocalForward(const Poly::EntityTransform& transform);
	::pe::core::math::Vector GetLocalRight(const Poly::EntityTransform& transform);
	::pe::core::math::Vector GetLocalUp(const Poly::EntityTransform& transform);

	::pe::core::math::Vector GetGlobalForward(const Poly::EntityTransform& transform);
	::pe::core::math::Vector GetGlobalRight(const Poly::EntityTransform& transform);
	::pe::core::math::Vector GetGlobalUp(const Poly::EntityTransform& transform);
}