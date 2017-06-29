#pragma once

#include "ResourceBase.hpp"

typedef unsigned int ALuint;

namespace Poly 
{

	class ENGINE_DLLEXPORT OggDecoderException : public BaseObject<>, public std::exception
	{
	public:
		OggDecoderException() {}
	};
	
	/// Resource that stores sound resource
	/// For now is designed only for opening ogg files
	class ENGINE_DLLEXPORT SoundResource : public ResourceBase
	{
	public:
		SoundResource(const String& path);
		~SoundResource();

		ALuint GetBufferID() const { return BufferID; }

	private:
		ALuint BufferID;
	};

} // namespace Poly

