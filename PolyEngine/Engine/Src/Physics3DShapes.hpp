#pragma once

namespace Poly
{
	enum class ePhysics3DShapes
	{
		PLANE,
		BOX,
		SPHERE,
		CAPSULE,
		_COUNT
	};

	class ENGINE_DLLEXPORT Physics3DShape
	{
	public:
		Physics3DShape(ePhysics3DShapes type) : ShapeType(type) {}
		const ePhysics3DShapes ShapeType;
	};

	class ENGINE_DLLEXPORT Physics3DPlaneShape : Physics3DShape
	{
	public:
		Physics3DPlaneShape(Vector normal, float halfExtent) : Physics3DShape(ePhysics3DShapes::PLANE), Normal(normal), HalfExtent(halfExtent) {}
		Vector Normal;
		float HalfExtent;
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

	class ENGINE_DLLEXPORT Physics3DCapsuleShape : Physics3DShape
	{
	public:
		Physics3DCapsuleShape(float radius, float height) : Physics3DShape(ePhysics3DShapes::PLANE), Radius(radius), Height(height) {}
		float Radius;
		float Height;
	};
}