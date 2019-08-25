#include <EnginePCH.hpp>

#include <Physics3D/Physics3DShapes.hpp>
#include <Physics3D/Physics3DShapesImpl.hpp>
#include <Resources/ResourceManager.hpp>

//------------------------------------------------------------------------------
Poly::Physics3DShape::~Physics3DShape()
{
	if (ImplData)
	{
		delete ImplData->BulletShape;
		delete ImplData;
	}
		
}

//------------------------------------------------------------------------------
Poly::Physics3DStaticMeshSource::Physics3DStaticMeshSource()
{
	ImplData = std::make_unique<Physics3DStaticMeshSourceImpl>();
	ImplData->BulletMesh = new btTriangleMesh();
}

//------------------------------------------------------------------------------

Poly::Physics3DStaticMeshSource::~Physics3DStaticMeshSource()
{
	ImplData.release();
}

//------------------------------------------------------------------------------
void Poly::Physics3DStaticMeshSource::LoadMesh(const core::storage::String& meshPath, eResourceSource source)
{
	MeshResource* resource = ResourceManager<MeshResource>::Load(meshPath, source);
	const std::vector<MeshResource::SubMesh*>& subMeshes = resource->GetSubMeshes();

	for (size_t m = 0; m < subMeshes.size(); ++m)
	{
		LoadMesh(*subMeshes[m]);
	}

	ResourceManager<MeshResource>::Release(resource);
}

//------------------------------------------------------------------------------
void Poly::Physics3DStaticMeshSource::LoadMesh(const MeshResource::SubMesh& subMesh)
{
	const Mesh& mesh = subMesh.GetMeshData();

	const std::vector<core::math::Vector3f>& positions = mesh.GetPositions();
	const std::vector<uint32_t>& indices = mesh.GetIndicies();

	for (size_t i = 0; i < indices.size();)
	{
		core::math::Vector a = positions[indices[i]].GetVector();
		core::math::Vector b = positions[indices[++i]].GetVector();
		core::math::Vector c = positions[indices[++i]].GetVector();
		++i;

		AddTriangle(a, b, c);
	}
}

//------------------------------------------------------------------------------
void Poly::Physics3DStaticMeshSource::AddTriangle(const core::math::Vector& a, const core::math::Vector& b, const core::math::Vector& c)
{
	ImplData->BulletMesh->addTriangle({ a.X, a.Y, a.Z }, { b.X, b.Y, b.Z }, { c.X, c.Y, c.Z }, true);
}

//------------------------------------------------------------------------------
Poly::Physics3DStaticMeshShape::Physics3DStaticMeshShape(const Physics3DStaticMeshSource& source)
	: Physics3DShape(ePhysics3DShape::MESH)
{
	ImplData = new Physics3DShapeImpl();
	ImplData->BulletShape = new btBvhTriangleMeshShape(source.ImplData->BulletMesh, true);
}

//------------------------------------------------------------------------------
Poly::Physics3DPlaneShape::Physics3DPlaneShape(core::math::Vector normal, float halfExtent) 
	: Physics3DShape(ePhysics3DShape::PLANE), 
	Normal(normal), 
	HalfExtent(halfExtent)
{
	ImplData = new Physics3DShapeImpl();
	ImplData->BulletShape = new btStaticPlaneShape(btVector3(normal.X, normal.Y, normal.Z), halfExtent);
}

//------------------------------------------------------------------------------
Poly::Physics3DBoxShape::Physics3DBoxShape(core::math::Vector halfExtents)
	: Physics3DShape(ePhysics3DShape::BOX),
	HalfExtents(halfExtents)
{
	ImplData = new Physics3DShapeImpl();
	ImplData->BulletShape = new btBoxShape(btVector3(halfExtents.X, halfExtents.Y, halfExtents.Z));
}

//------------------------------------------------------------------------------
Poly::Physics3DSphereShape::Physics3DSphereShape(float radius)
	: Physics3DShape(ePhysics3DShape::SPHERE),
	Radius(radius)
{
	ImplData = new Physics3DShapeImpl();
	ImplData->BulletShape = new btSphereShape(radius);
}

//------------------------------------------------------------------------------
Poly::Physics3DCapsuleShape::Physics3DCapsuleShape(float radius, float height)
	: Physics3DShape(ePhysics3DShape::PLANE), 
	Radius(radius), 
	Height(height)
{
	ImplData = new Physics3DShapeImpl();
	ImplData->BulletShape = new btCapsuleShape(radius, height);
}
