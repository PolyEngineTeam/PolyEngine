#pragma once

class btCollisionShape;

namespace Poly
{
	enum class ePhysics3DShape
	{
		PLANE,
		BOX,
		SPHERE,
		CAPSULE,
		_COUNT
	};

	class ENGINE_DLLEXPORT Physics3DShape
	{
		friend class Trigger3DComponent;
		friend class Rigidbody3DComponent;
	public:
		Physics3DShape(ePhysics3DShape type) : ShapeType(type) {}
		const ePhysics3DShape ShapeType;
	protected:
		btCollisionShape* BulletShape;
	};

	class ENGINE_DLLEXPORT Physics3DPlaneShape : public Physics3DShape
	{
	public:
		Physics3DPlaneShape(Vector normal, float halfExtent);
		const Vector Normal;
		const float HalfExtent;
	};

	class ENGINE_DLLEXPORT Physics3DBoxShape : public Physics3DShape
	{
	public:
		Physics3DBoxShape(Vector halfExtents);
		const Vector HalfExtents;
	};

	class ENGINE_DLLEXPORT Physics3DSphereShape : public Physics3DShape
	{
	public:
		Physics3DSphereShape(float radius);
		const float Radius;
	};

	class ENGINE_DLLEXPORT Physics3DCapsuleShape : public Physics3DShape
	{
	public:
		Physics3DCapsuleShape(float radius, float height);
		const float Radius;
		const float Height;
	};
}