#pragma once
#include "Vector.hpp"

namespace Poly
{
	class World;

	namespace DebugRenderingSystem
	{
		void ENGINE_DLLEXPORT DebugRenderingUpdatePhase(World*);

		// Emitters of elementary shapes
		void EmitPoint(Vector position, float size);
		void EmitLine(World* world, Vector begin, Vector end);
		void EmitQuad(Vector mins, Vector maxs);
		void EmitBox(Vector mins, Vector maxs);
		void EmitSphere(Vector position, float radius);
		void EmitArrow(Vector position, Vector direction);
	}
}