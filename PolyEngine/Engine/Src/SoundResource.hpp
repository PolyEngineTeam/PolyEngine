#pragma once

#include "ResourceBase.hpp"

typedef unsigned int ALuint;

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