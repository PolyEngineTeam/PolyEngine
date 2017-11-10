#include "EnginePCH.hpp"

using namespace Poly;

SoundResource::SoundResource(const String& path, eSoundFileFormat format, size_t size, size_t offset)
{
	SoundSystem::DecodeSoundData(this, *LoadBinaryFile(path), format, size, offset);
}

SoundResource::~SoundResource()
{
	if (RawData)
		delete RawData;
}
