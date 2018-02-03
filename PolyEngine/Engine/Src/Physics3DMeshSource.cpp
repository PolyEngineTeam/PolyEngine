#include "EnginePCH.hpp"

#include "Physics3DMeshSource.hpp"

#include "Physics3DMeshSourceImpl.hpp"

//------------------------------------------------------------------------------
Poly::Physics3DMeshSource::Physics3DMeshSource()
{
	ImplData->BulletMesh = new btTriangleMesh();
}

//------------------------------------------------------------------------------
Poly::Physics3DMeshSource::~Physics3DMeshSource()
{
	if (ImplData)
		delete ImplData->BulletMesh;
}

//------------------------------------------------------------------------------
void Poly::Physics3DMeshSource::AddTriangle(const Vector& a, const Vector& b, const Vector& c)
{
	ImplData->BulletMesh->addTriangle({ a.X, a.Y, a.Z }, { b.X, b.Y, b.Z }, { c.X, c.Y, c.Z }, true);
}