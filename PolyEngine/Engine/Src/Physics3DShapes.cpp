#include "Physics3DShapes.hpp"

#include "Physics3DShapeImpl.hpp"
#include "Physics3DMeshSourceImpl.hpp"

//------------------------------------------------------------------------------
Poly::Physics3DShape::Physics3DShape(const Physics3DShape& shape)
	: ShapeType(shape.ShapeType)
{
	// FIXME(squares): why do we need this constructor
}

//------------------------------------------------------------------------------
Poly::Physics3DShape::~Physics3DShape()
{
	if (ImplData)
		delete ImplData->BulletShape;
}

//------------------------------------------------------------------------------
Poly::Physics3DMeshShape::Physics3DMeshShape(const Physics3DMeshSource& source)
	: Physics3DShape(ePhysics3DShape::MESH)
{
	ImplData->BulletShape = new btBvhTriangleMeshShape(source.ImplData->BulletMesh, true);
}

//------------------------------------------------------------------------------
Poly::Physics3DPlaneShape::Physics3DPlaneShape(Vector normal, float halfExtent)
	: Physics3DShape(ePhysics3DShape::PLANE),
	Normal(normal),
	HalfExtent(halfExtent)
{
	ImplData->BulletShape = new btStaticPlaneShape(btVector3(normal.X, normal.Y, normal.Z), halfExtent);
}

//------------------------------------------------------------------------------
Poly::Physics3DBoxShape::Physics3DBoxShape(Vector halfExtents)
	: Physics3DShape(ePhysics3DShape::BOX),
	HalfExtents(halfExtents)
{
	ImplData->BulletShape = new btBoxShape(btVector3(halfExtents.X, halfExtents.Y, halfExtents.Z));
}

//------------------------------------------------------------------------------
Poly::Physics3DSphereShape::Physics3DSphereShape(float radius)
	: Physics3DShape(ePhysics3DShape::SPHERE),
	Radius(radius)
{
	ImplData->BulletShape = new btSphereShape(radius);
}

//------------------------------------------------------------------------------
Poly::Physics3DCapsuleShape::Physics3DCapsuleShape(float radius, float height)
	: Physics3DShape(ePhysics3DShape::PLANE),
	Radius(radius),
	Height(height)
{
	ImplData->BulletShape = new btCapsuleShape(radius, height);
}
