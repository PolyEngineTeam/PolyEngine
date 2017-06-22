#include "EnginePCH.hpp"

#include "SoundWorldComponent.hpp"

#include <alc.h>

using namespace Poly;

SoundWorldComponent::SoundWorldComponent(World* world)
{
	Context = alcCreateContext(world->GetEngine()->GetAudioRenderingDevice().Device, NULL);
}

SoundWorldComponent::~SoundWorldComponent()
{
	alcDestroyContext(Context);
}
