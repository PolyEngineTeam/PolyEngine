#pragma once

#include <pe/Defines.hpp>

class btCollisionShape;
class btTriangleMesh;

namespace Poly
{
	struct Physics3DShapeImpl : public ::pe::core::BaseObject<>
	{
		btCollisionShape* BulletShape = nullptr;
	};

	struct Physics3DStaticMeshSourceImpl : public ::pe::core::BaseObject<>
	{
		btTriangleMesh* BulletMesh = nullptr;
	};
}