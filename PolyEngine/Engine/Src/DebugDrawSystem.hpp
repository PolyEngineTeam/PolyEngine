#pragma once
#include "Vector.hpp"

namespace Poly
{
	class World;

	namespace DebugDrawSystem
	{
		void ENGINE_DLLEXPORT DebugRenderingUpdatePhase(World*);

		/// Emitters of elementary shapes
		void EmitLine(World* world, Vector begin, Vector end);
		void EmitBox(World* world, Vector mins, Vector maxs);

		/// Think of orientation as of normal vector
		void EmitCircle(World* world, Vector position, float radius, Vector orientation);
		void EmitSphere(World* world, Vector position, float radius);
		void EmitArrow(World* world, Vector position, Vector directionVector);
	}
}