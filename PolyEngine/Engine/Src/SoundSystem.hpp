#pragma once

#include "UniqueID.hpp"

namespace Poly
{
	class World;
	class SoundResource;

	namespace SoundSystem
	{
		void SoundUpdatePhase(World*);

		void BindSoundResourceToEmitter(World*, UniqueID, SoundResource*);
	}
}
