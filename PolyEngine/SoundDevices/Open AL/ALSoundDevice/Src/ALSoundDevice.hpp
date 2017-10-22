#pragma once

#include <ISoundDevice.hpp>
#include <UniqueID.hpp>
#include <Dynarray.hpp>

namespace Poly
{
	class SOUND_DEVICE_DLLEXPORT ALSoundDevice : ISoundDevice
	{
	public:
		void RenderWorld(World* world);

	private:
		Dynarray<UniqueID> EmitterOwners;
		std::unordered_map<UniqueID, unsigned int> OwnerIDToEmitterIDMap;
	};
}