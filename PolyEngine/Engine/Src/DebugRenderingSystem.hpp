#pragma once
#include "Vector.hpp"

// TODO:
// rename. there is no actual 'rendering' in this system

namespace Poly
{
	class World;

	namespace DebugRenderingSystem
	{
		void ENGINE_DLLEXPORT DebugRenderingUpdatePhase(World*);

		// Emitters of elementary shapes
		void EmitPoint(Vector position, float size);
		void EmitLine(World* world, Mesh::Vector3D begin, Mesh::Vector3D end);
		void EmitQuad(Vector mins, Vector maxs);
		void EmitBox(World* world, Mesh::Vector3D mins, Mesh::Vector3D maxs);
		void EmitSphere(Vector position, float radius);
		void EmitArrow(Vector position, Vector direction);
	}
}