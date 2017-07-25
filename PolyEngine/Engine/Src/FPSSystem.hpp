#pragma once

namespace Poly
{
	class World;

	namespace FPSSystem
	{
		struct FPSData
		{
			int FPS;
			double ElapsedTime;
			bool DisplayingFPS;
		};

		float ENGINE_DLLEXPORT GetFPS(World* world);
		void FPSUpdatePhase(World*);
	}
}