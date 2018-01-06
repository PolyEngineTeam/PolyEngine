#pragma once
#include "Vector.hpp"

namespace Poly
{
	class World;

	namespace DebugDrawSystem
	{
		void ENGINE_DLLEXPORT DebugRenderingUpdatePhase(World*);

		/// Emitters of elementary shapes
		void DrawLine(World* world, Vector begin, Vector end);
		void DrawBox(World* world, Vector mins, Vector maxs);

		/// Think of orientation as of normal vector
		void DrawCircle(World* world, Vector position, float radius, Vector orientation);
		void DrawSphere(World* world, Vector position, float radius);
		void DrawArrow(World* world, Vector position, Vector directionVector);
	}
}