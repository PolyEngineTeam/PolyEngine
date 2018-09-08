#pragma once

#include <Defines.hpp>

namespace Poly
{
	class Scene;
}

namespace EditorCameraMovementSystem
{
	void Update(Poly::Scene* scene);

	Vector GetLocalForward(const Poly::EntityTransform& transform);
	Vector GetLocalRight(const Poly::EntityTransform& transform);
	Vector GetLocalUp(const Poly::EntityTransform& transform);

	Vector GetGlobalForward(const Poly::EntityTransform& transform);
	Vector GetGlobalRight(const Poly::EntityTransform& transform);
	Vector GetGlobalUp(const Poly::EntityTransform& transform);
}