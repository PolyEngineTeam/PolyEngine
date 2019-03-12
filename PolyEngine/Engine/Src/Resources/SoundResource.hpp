#pragma once

#include <Defines.hpp>
#include <Resources/ResourceBase.hpp>

namespace Poly 
{

	SILENCE_MSVC_WARNING(4275, "Exporting stl may cause incompatibility. We use same CRT so it's ok.")
	class ENGINE_DLLEXPORT OggDecoderException : public BaseObject<>, public std::exception
	{
	public:
		OggDecoderException() {}
	};
	UNSILENCE_MSVC_WARNING()
	
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

