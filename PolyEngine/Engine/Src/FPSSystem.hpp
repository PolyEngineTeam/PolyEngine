#pragma once

namespace Poly
{
	class World;

	namespace FPSSystem
	{
		void FPSUpdatePhase(World*);

		float GetFPS(World* world);
	}
}