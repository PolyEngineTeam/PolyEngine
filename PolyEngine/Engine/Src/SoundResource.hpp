#pragma once

// engine
#include "SoundSystem.hpp"
#include "ResourceBase.hpp"

namespace Poly
{
	class BinaryBuffer;

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

	class ENGINE_DLLEXPORT SoundResource : public ResourceBase
	{
		friend void SoundSystem::DecodeOggVorbis(SoundResource*, const BinaryBuffer&, size_t, size_t);
	public:
		SoundResource(const String& path, eSoundFileFormat format = eSoundFileFormat::OGG_VORBIS, size_t size = 0, size_t offset = 0);
		~SoundResource() override;

		const BinaryBuffer* GetRawData() const { return RawData; }
		const BinaryBuffer* GetRawData() { return RawData; }
		eSoundSampleFormat GetSampleFormat() const { return SampleFormat; }
		eSoundSampleFormat GetSampleFormat() { return SampleFormat; }
		size_t GetFrequency() const { return Frequency; }
		size_t GetFrequency() { return Frequency; }

	private:
		BinaryBuffer* RawData;
		eSoundSampleFormat SampleFormat;
		size_t Frequency;
	};

} // namespace Poly

