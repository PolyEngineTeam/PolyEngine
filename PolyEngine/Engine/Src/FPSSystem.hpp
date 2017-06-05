#pragma once

namespace Poly
{
	class World;

	namespace FPSSystem
	{
		void ENGINE_DLLEXPORT FPSUpdatePhase(World*);

		float ENGINE_DLLEXPORT GetFPS(World* world);
	}
}