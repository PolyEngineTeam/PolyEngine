#pragma once

#include "Physics3DSystem.hpp"

	// FIXME(squares): this shouldn't be declared here
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

	class ENGINE_DLLEXPORT Physics3DShape : public BaseObject<>
	{
		//friend void Physics3DSystem::EnsureInit(World* world, const UniqueID& entityID);
		//friend const Vector& Physics3DSystem::CalculateIntertia(const Physics3DShape& shape, float mass);
	public:
		Physics3DShape(ePhysics3DShape type) : ShapeType(type) {}
		virtual ~Physics3DShape();
		const ePhysics3DShape ShapeType;
			// FIXME(squares): friendship problems
		btCollisionShape* BulletShape = nullptr;
	protected:
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
} // namespace Poly