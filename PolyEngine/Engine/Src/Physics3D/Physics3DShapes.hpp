#pragma once

#include <Defines.hpp>
#include <Resources/MeshResource.hpp>
#include <Physics3D/Physics3DSystem.hpp>
#include <Physics3D/Collider3DComponent.hpp>

namespace Poly
{
	struct Physics3DShapeImpl;
	struct Physics3DStaticMeshSourceImpl;

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


	class ENGINE_DLLEXPORT Physics3DStaticMeshSource : public BaseObject<>
	{
		friend class Physics3DStaticMeshShape;
	public:
		Physics3DStaticMeshSource();
		~Physics3DStaticMeshSource();
		void LoadMesh(const String& meshPath, eResourceSource source);
		void LoadMesh(const MeshResource::SubMesh& subMesh);
		void AddTriangle(const Vector& a, const Vector& b, const Vector& c);

	private:
		std::unique_ptr<Physics3DStaticMeshSourceImpl> ImplData;
	};


	//------------------------------------------------------------------------------


	class ENGINE_DLLEXPORT Physics3DShape : public BaseObject<>
	{
		friend void Physics3DSystem::EnsureInit(Scene* world, Entity* entity);
		friend Vector Physics3DSystem::CalculateInertia(const Physics3DShape* shape, float mass);
		friend void Collider3DComponent::SetShape(const Physics3DShape* shape);
	public:
		Physics3DShape(ePhysics3DShape type) : ShapeType(type) {}
		virtual ~Physics3DShape();
		const ePhysics3DShape ShapeType;

	protected:
		Physics3DShapeImpl* ImplData;
	};


	//------------------------------------------------------------------------------

	class ENGINE_DLLEXPORT Physics3DStaticMeshShape : public Physics3DShape
	{
	public:
		Physics3DStaticMeshShape(const Physics3DStaticMeshSource& source);
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