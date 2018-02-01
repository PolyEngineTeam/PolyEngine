#include "EnginePCH.hpp"

SILENCE_GCC_WARNING(-Werror, "Bullet includes has warnings")
	#include <btBulletCollisionCommon.h>
UNSILENCE_GCC_WARNING()

//------------------------------------------------------------------------------
Poly::Physics3DShape::~Physics3DShape()
{
	if (BulletShape)
		delete BulletShape;
}

//------------------------------------------------------------------------------
Poly::Physics3DPlaneShape::Physics3DPlaneShape(Vector normal, float halfExtent) 
	: Physics3DShape(ePhysics3DShape::PLANE), 
	Normal(normal), 
	HalfExtent(halfExtent)
{
	BulletShape = new btStaticPlaneShape(btVector3(normal.X, normal.Y, normal.Z), halfExtent);
}

//------------------------------------------------------------------------------
Poly::Physics3DBoxShape::Physics3DBoxShape(Vector halfExtents)
	: Physics3DShape(ePhysics3DShape::BOX),
	HalfExtents(halfExtents)
{
	BulletShape = new btBoxShape(btVector3(halfExtents.X, halfExtents.Y, halfExtents.Z));
}

//------------------------------------------------------------------------------
Poly::Physics3DSphereShape::Physics3DSphereShape(float radius)
	: Physics3DShape(ePhysics3DShape::SPHERE),
	Radius(radius)
{
	BulletShape = new btSphereShape(radius);
}

//------------------------------------------------------------------------------
Poly::Physics3DCapsuleShape::Physics3DCapsuleShape(float radius, float height)
	: Physics3DShape(ePhysics3DShape::PLANE), 
	Radius(radius), 
	Height(height)
{
	BulletShape = new btCapsuleShape(radius, height);
}
