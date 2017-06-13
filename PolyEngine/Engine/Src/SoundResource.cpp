#include "EnginePCH.hpp"

#include "SoundResource.hpp"

using namespace Poly;

SoundResource::SoundResource(const String& path)
{
	alGenBuffers(1, &BufferID);

	// TODO: Get raw data here
	float freq = 440.f;
	int seconds = 4;
	unsigned sample_rate = 22050;
	size_t buf_size = seconds * sample_rate;

	short *samples;
	samples = new short[buf_size];
	for(int i=0; i<buf_size; ++i) {
		samples[i] = 32760 * sin( (2.f*float(M_PI)*freq)/sample_rate * i );
	}

	alBufferData(BufferID, AL_FORMAT_MONO16, samples, buf_size, sample_rate);
}

SoundResource::~SoundResource()
{	
	alDeleteBuffers(1, &BufferID);
}
