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
		void EmitPoint(World* world, Mesh::Vector3D position, float size);
		void EmitLine(World* world, Vector begin, Vector end);
		void EmitQuad(Mesh::Vector3D mins, Mesh::Vector3D maxs);
		void EmitBox(World* world, Vector mins, Vector maxs);
		void EmitSphere(World* world, Mesh::Vector3D position, float radius);
		void EmitArrow(World* world, Vector position, Vector directionVector);
	}
}