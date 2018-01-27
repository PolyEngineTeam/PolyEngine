#include "EnginePCH.hpp"

#include <btBulletCollisionCommon.h>

//------------------------------------------------------------------------------
Poly::Physics3DShape::~Physics3DShape()
{
	if (BulletShape)
		delete BulletShape;
}

//------------------------------------------------------------------------------
Poly::Physics3DTriangleMeshSource::Physics3DTriangleMeshSource()
{
	BulletMesh = new btTriangleMesh();
}

//------------------------------------------------------------------------------
void Poly::Physics3DTriangleMeshSource::LoadMesh(const String& meshPath, eResourceSource source)
{
	MeshResource* resource = ResourceManager<MeshResource>::Load(meshPath, source);
	const Dynarray<MeshResource::SubMesh*>& subMeshes = resource->GetSubMeshes();

	for (int m = 0; m < subMeshes.GetSize(); ++m)
	{
		LoadMesh(*subMeshes[m]);
	}

	ResourceManager<MeshResource>::Release(resource);
}

//------------------------------------------------------------------------------
void Poly::Physics3DTriangleMeshSource::LoadMesh(const MeshResource::SubMesh& subMesh)
{
	const Mesh& mesh = subMesh.GetMeshData();

	const Dynarray<Vector3f>& positions = mesh.GetPositions();
	const Dynarray<uint32_t>& indices = mesh.GetIndicies();

	for (int i = 0; i < indices.GetSize();)
	{
		Vector a = positions[indices[i]].GetVector();
		Vector b = positions[indices[++i]].GetVector();
		Vector c = positions[indices[++i]].GetVector();
		++i;

		AddTriangle(a, b, c);
	}
}

//------------------------------------------------------------------------------
void Poly::Physics3DTriangleMeshSource::AddTriangle(const Vector& a, const Vector& b, const Vector& c)
{
	BulletMesh->addTriangle({ a.X, a.Y, a.Z }, { b.X, b.Y, b.Z }, { c.X, c.Y, c.Z }, true);
}

//------------------------------------------------------------------------------
Poly::Physics3DTriangleMeshShape::Physics3DTriangleMeshShape(const Physics3DTriangleMeshSource& source)
	: Physics3DShape(ePhysics3DShape::MESH)
{
	BulletShape = new btBvhTriangleMeshShape(source.BulletMesh, true);
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
