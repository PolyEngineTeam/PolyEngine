#pragma once

#include <SoundDataComponentProxy.hpp>

namespace Poly
{
	class SoundDataComponent : public SoundDataComponentProxy
	{
	public:
		SoundDataComponent();
		~SoundDataComponent();
		
		unsigned int EmitterID;
		Poly::Dynarray<unsigned int> QueuedBuffers;
		unsigned int MaxBuffersInQueue = 2;
	};
}