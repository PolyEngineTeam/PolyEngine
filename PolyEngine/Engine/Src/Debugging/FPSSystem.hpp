#pragma once

#include <pe/Defines.hpp>

namespace Poly
{
	class Scene;

	namespace FPSSystem
	{
		struct FPSData : public ::pe::core::BaseObject<>
		{
			int FPS;
			double ElapsedTime;
			int LastFPS;
			::pe::core::storage::String DisplayedFPSText;
		};

		float ENGINE_DLLEXPORT GetFPS(Scene* world);
		void FPSUpdatePhase(Scene*);
	}
}