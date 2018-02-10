#pragma once

class btCollisionShape;
class btTriangleMesh;

namespace Poly
{
	struct Physics3DShapeImpl
	{
		btCollisionShape* BulletShape = nullptr;
	};

	struct Physics3DStaticMeshSourceImpl
	{
		btTriangleMesh* BulletMesh = nullptr;
	};
}