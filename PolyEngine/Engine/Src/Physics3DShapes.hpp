#pragma once

#include "Physics3DSystem.hpp"
#include "Physics3DMeshSource.hpp"
#include "Physics3DShapeImpl.hpp"

namespace Poly
{
	struct Physics3DShapeImplData;


	//------------------------------------------------------------------------------

	enum class ePhysics3DShape
	{
		MESH,
		PLANE,
		BOX,
		SPHERE,
		CAPSULE,
		_COUNT
	};

	//------------------------------------------------------------------------------

	class ENGINE_DLLEXPORT Physics3DShape : public BaseObject<>
	{
		friend class Collider3DComponent;
		friend void Physics3DSystem::EnsureInit(World* world, Entity* entity);
		friend Vector Physics3DSystem::CalculateIntertia(const Physics3DShape* shape, float mass);

	public:
		Physics3DShape(ePhysics3DShape type) : ShapeType(type) {}
		Physics3DShape(const Physics3DShape& shape);
		virtual ~Physics3DShape();

		const ePhysics3DShape ShapeType;

	protected:
		std::unique_ptr<Physics3DShapeImplData> ImplData;
	};

	//------------------------------------------------------------------------------

	class ENGINE_DLLEXPORT Physics3DMeshShape : public Physics3DShape
	{
	public:
		Physics3DMeshShape(const Physics3DMeshSource& source);
	};

	//------------------------------------------------------------------------------

	class ENGINE_DLLEXPORT Physics3DPlaneShape : public Physics3DShape
	{
	public:
		Physics3DPlaneShape(Vector normal, float halfExtent);
		const Vector Normal;
		const float HalfExtent;
	};

	//------------------------------------------------------------------------------

	class ENGINE_DLLEXPORT Physics3DBoxShape : public Physics3DShape
	{
	public:
		Physics3DBoxShape(Vector halfExtents);
		const Vector HalfExtents;
	};

	//------------------------------------------------------------------------------

	class ENGINE_DLLEXPORT Physics3DSphereShape : public Physics3DShape
	{
	public:
		Physics3DSphereShape(float radius);
		const float Radius;
	};

	//------------------------------------------------------------------------------

	class ENGINE_DLLEXPORT Physics3DCapsuleShape : public Physics3DShape
	{
	public:
		Physics3DCapsuleShape(float radius, float height);
		const float Radius;
		const float Height;
	};

} // namespace Poly