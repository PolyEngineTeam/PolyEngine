#include "EnginePCH.hpp"

#include <al.h> 
#include <alc.h>

#include "SoundSystem.hpp"

using namespace Poly;

void SoundSystem::SoundPhase(World* world)
{
	SoundEmitterComponent* emitter;
	ALint state;

	for (auto it : world->IterateComponents<SoundEmitterComponent>())
	{
		emitter = std::get<SoundEmitterComponent*>(it);
		alGetSourcei(emitter->SourceID, AL_SOURCE_STATE, &state);

		if (state != AL_PLAYING)
			alSourcePlay(emitter->SourceID);
	}

	
	/*ALCdevice *device;
	ALCcontext *context;

	device = alcOpenDevice(NULL);
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	//*****************************************************************************************

	ALuint buffer;

	alGenBuffers(1, &buffer);

	float freq = 440.f;
	int seconds = 4;
	unsigned sample_rate = 22050;
	size_t buf_size = seconds * sample_rate;

	short *samples;
	samples = new short[buf_size];
	for(int i=0; i<buf_size; ++i) {
		samples[i] = 32760 * sin( (2.f*float(M_PI)*freq)/sample_rate * i );
	}

	alBufferData(buffer, AL_FORMAT_MONO16, samples, buf_size, sample_rate);

	//*****************************************************************************************

	ALuint source = 0;
	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER, buffer);
	alSourcePlay(source);

	//*****************************************************************************************

	ALint state;

	do {
		alGetSourcei(source, AL_SOURCE_STATE, &state);
	} while (state == AL_PLAYING);

	//*****************************************************************************************

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
	*/
}

void SoundSystem::CreateContext(World* world)
{
	world->GetWorldComponent<SoundWorldComponent>()->Context = alcCreateContext(world->GetEngine()->GetAudioRenderingContext().GetDevice(), NULL);
}

void SoundSystem::SetCurrentWorld(World* world)
{
	ALCcontext *context;
	context = world->GetWorldComponent<SoundWorldComponent>()->Context;
	alcMakeContextCurrent(context);
}

void SoundSystem::BindSoundResourceToEmitter(World *, UniqueID, SoundResource *)
{
}
