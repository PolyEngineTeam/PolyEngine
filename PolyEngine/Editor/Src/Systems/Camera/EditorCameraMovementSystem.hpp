#pragma once

#include <Core.hpp>

namespace Poly
{
	class World;
	class EntityTransform;
}
	
using namespace Poly;

namespace EditorCameraMovementSystem
{
	void Update(World* world);

	Vector GetLocalForward(const EntityTransform& transform);
	Vector GetLocalRight(const EntityTransform& transform);
	Vector GetLocalUp(const EntityTransform& transform);

	Vector GetGlobalForward(const EntityTransform& transform);
	Vector GetGlobalRight(const EntityTransform& transform);
	Vector GetGlobalUp(const EntityTransform& transform);
}