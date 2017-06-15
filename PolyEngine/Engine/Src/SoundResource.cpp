#include "EnginePCH.hpp"

#include <al.h>

#include "SoundResource.hpp"

using namespace Poly;

SoundResource::SoundResource(const String& path)
{
	alGenBuffers(1, &BufferID);

	// TODO: Get raw data here
	/*float freq = 20.f;
	int seconds = 1;
	unsigned sample_rate = 2000;
	size_t buf_size = seconds * sample_rate;

	short *samples;
	samples = new short[buf_size];
	for(int i=0; i<buf_size; ++i) 
		samples[i] = 20000 * sin( (2.f*float(M_PI) * freq) / sample_rate * i );*/

	float freq = 200.f;
	int seconds = 7 * 2;
	unsigned sample_rate = 192000;
	size_t buf_size = seconds * sample_rate;

	short *samples;
	samples = new short[buf_size];
	for(int i = 0; i < buf_size; ++i) 
		samples[i] = 20000 * sin((2.f * i * i * float(M_PI) ) * (freq / sample_rate));

	alBufferData(BufferID, AL_FORMAT_MONO16, samples, buf_size * sizeof(short), sample_rate);
}

SoundResource::~SoundResource()
{	
	alDeleteBuffers(1, &BufferID);
}
