#pragma once

#include <BaseObject.hpp>

namespace Poly
{
	class World;

	namespace FPSSystem
	{
		struct FPSData : public BaseObject<>
		{
			int FPS;
			double ElapsedTime;
			int LastFPS;
			String DisplayedFPSText;
		};

		float ENGINE_DLLEXPORT GetFPS(World* world);
		void FPSUpdatePhase(World*);
	}
}