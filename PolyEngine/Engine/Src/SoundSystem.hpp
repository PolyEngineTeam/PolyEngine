#pragma once

#include "UniqueID.hpp"

namespace Poly
{
	class World;
	class SoundResource;

	namespace SoundSystem
	{
		void SoundPhase(World*);

		void CreateContext(World*);
		void SetCurrentWorld(World*);
		void BindSoundResourceToEmitter(World*, UniqueID, SoundResource*);
	}
}
