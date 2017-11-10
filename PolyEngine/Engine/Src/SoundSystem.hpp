#pragma once

namespace Poly
{
	class ENGINE_DLLEXPORT SoundSystemException : public BaseObject<>, public std::exception
	{
	public:
		SoundSystemException() {}
	};

	class ENGINE_DLLEXPORT OggDecoderException : public SoundSystemException
	{
	public:
		OggDecoderException() {}
	};

	class World;
	class BinaryBuffer;
	class SoundResource;
	enum class eSoundFileFormat;

	namespace SoundSystem
	{
		void ENGINE_DLLEXPORT SoundPhase(World*);

		void ENGINE_DLLEXPORT DecodeSoundData(SoundResource* resource, const BinaryBuffer& data, eSoundFileFormat format, size_t size, size_t offset);

		void ENGINE_DLLEXPORT DecodeOggVorbis(SoundResource* resource, const BinaryBuffer& data, size_t size, size_t offset);
	}
} // namespace Poly
