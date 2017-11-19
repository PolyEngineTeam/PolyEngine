#pragma once

#include <ISoundDevice.hpp>

namespace Poly
{
	class SoundEmitterData : public ISoundEmitterDataProxy
	{
	public:
		SoundEmitterData();

		unsigned int EmitterID;
		Dynarray<unsigned int> CurrentPlaylist;
	};
}