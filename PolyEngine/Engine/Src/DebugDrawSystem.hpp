#pragma once
#include "Vector.hpp"

namespace Poly
{
	class World;

	namespace DebugDrawSystem
	{
		void ENGINE_DLLEXPORT DebugRenderingUpdatePhase(World*);

		/// Emitters of elementary shapes
		void ENGINE_DLLEXPORT DrawLine(World* world, Vector begin, Vector end);
		void ENGINE_DLLEXPORT DrawBox(World* world, Vector mins, Vector maxs);

		/// Think of orientation as of normal vector
		void ENGINE_DLLEXPORT DrawCircle(World* world, Vector position, float radius, Vector orientation);
		void ENGINE_DLLEXPORT DrawSphere(World* world, Vector position, float radius);
		void ENGINE_DLLEXPORT DrawArrow(World* world, Vector position, Vector directionVector);
	}
}