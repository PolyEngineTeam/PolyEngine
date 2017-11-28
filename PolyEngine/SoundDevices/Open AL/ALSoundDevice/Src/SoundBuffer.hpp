#pragma once

#include <ISoundDevice.hpp>

namespace Poly
{
	class SoundBuffer : public ISoundBufferProxy
	{
	public:
		SoundBuffer();

		unsigned int BufferID;
	};
} // namespace Poly