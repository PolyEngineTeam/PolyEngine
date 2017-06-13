#pragma once

#include <al.h>

#include "ResourceBase.hpp"

namespace Poly 
{
	class ENGINE_DLLEXPORT SoundResource : public ResourceBase
	{
	public:
		SoundResource(const String& path);
		~SoundResource();

		ALuint GetBufferID() const { return BufferID; }

	private:
		ALuint BufferID;
	};
}