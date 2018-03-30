#pragma once

#include "Resources/ResourceBase.hpp"

namespace Poly 
{

	class ENGINE_DLLEXPORT OggDecoderException : public BaseObject<>, public std::exception
	{
	public:
		OggDecoderException() {}
	};
	
	/// Resource that stores sound resource
	/// For now is designed only for opening ogg files (and not too large (tested on 188KB sample))
	class ENGINE_DLLEXPORT SoundResource : public ResourceBase
	{
	public:
		SoundResource(const String& path);
		~SoundResource();

		unsigned int GetBufferID() const { return BufferID; }

	private:
		unsigned int BufferID;
	};

} // namespace Poly

