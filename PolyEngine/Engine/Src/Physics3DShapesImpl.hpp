#pragma once

#include <BaseObject.hpp>

class btCollisionShape;
class btTriangleMesh;

namespace Poly
{
	struct Physics3DShapeImpl : BaseObject<>
	{
		btCollisionShape* BulletShape = nullptr;
	};

	struct Physics3DStaticMeshSourceImpl : BaseObject<>
	{
		btTriangleMesh* BulletMesh = nullptr;
	};
}