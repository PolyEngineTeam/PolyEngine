#pragma once

#include "ResourceBase.hpp"

namespace Poly 
{

	class ENGINE_DLLEXPORT OggDecoderException : public BaseObject<>, public std::exception
	{
	public:
		OggDecoderException() {}
	};

	enum class eSoundSampleFormat 
	{
		MONO8 = 0,
		MONO16,
		STEREO8,
		STEREO16,
		_COUNT
	};
	
	/// Resource that stores sound resource
	/// For now is designed only for opening ogg files (and not too large (tested on 188KB sample))
	class ENGINE_DLLEXPORT SoundResource : public ResourceBase
	{
	public:
		// samples
		SoundResource(const String& path, size_t size = 0, size_t offset = 0);
		// seconds
		SoundResource(const String& path, float size = -1, float offset = 0);
		~SoundResource();

		void OggVorbisDecoder();

	private:
		Dynarray<char>* RawData;
		eSoundSampleFormat SampleFormat;
		size_t Frequency;
	};

} // namespace Poly

