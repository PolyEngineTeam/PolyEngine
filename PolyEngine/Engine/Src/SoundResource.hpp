#pragma once

#include "ResourceBase.hpp"

namespace Poly 
{
	class ENGINE_DLLEXPORT SoundResourceException : public BaseObject<>, public std::exception
	{
	public:
		SoundResourceException() {}
	};

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

	enum class eSoundFileFormat
	{
		OGG_VORBIS = 0,
		_COUNT
	};
	
	/// Resource that stores sound resource
	/// For now is designed only for opening ogg files (and not too large (tested on 188KB sample))
	class ENGINE_DLLEXPORT SoundResource : public ResourceBase
	{
	public:
		// samples
		SoundResource(const String& path, eSoundFileFormat format, size_t size = 0, size_t offset = 0);
		~SoundResource();

	private:
		void DecodeOggVorbis(const BinaryBuffer& data, size_t size, size_t offset);

		BinaryBuffer* RawData;
		eSoundSampleFormat SampleFormat;
		size_t Frequency;
	};

} // namespace Poly

