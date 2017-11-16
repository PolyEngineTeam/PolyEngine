#pragma once

#include <ISoundDevice.hpp>

namespace Poly
{
	class SoundDataHolder : public ISoundDataHolderProxy
	{
	public:
		SoundDataHolder(unsigned int emitterID);
		~SoundDataHolder();

		unsigned int EmitterID;
		Poly::Dynarray<unsigned int> QueuedBuffers;
		unsigned int MaxBuffersInQueue = 2;
	};
}