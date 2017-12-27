#pragma once

namespace Poly
{
	enum class ePhysics3DShapes
	{
		BOX,
		SPHERE,
		_COUNT
	};

	class ENGINE_DLLEXPORT Physics3DShape
	{
	public:
		Physics3DShape(ePhysics3DShapes type) : ShapeType(type) {}
		const ePhysics3DShapes ShapeType;
	};

	class ENGINE_DLLEXPORT Physics3DBoxShape : Physics3DShape
	{
	public:
		Physics3DBoxShape(Vector halfExtents) : Physics3DShape(ePhysics3DShapes::BOX), HalfExtents(halfExtents) {}
		Vector HalfExtents;
	};

	class ENGINE_DLLEXPORT Physics3DSphereShape : Physics3DShape
	{
	public:
		Physics3DSphereShape(float radius) : Physics3DShape(ePhysics3DShapes::SPHERE), Radius(radius) {}
		float Radius;
	};
}