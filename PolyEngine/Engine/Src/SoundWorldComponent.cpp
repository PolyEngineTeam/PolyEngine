#include "EnginePCH.hpp"

#include "SoundWorldComponent.hpp"

#include <alc.h>

using namespace Poly;

SoundWorldComponent::SoundWorldComponent(World* world)
{
	Context = alcCreateContext(gEngine->GetAudioRenderingDevice().Device, nullptr);
}

SoundWorldComponent::~SoundWorldComponent()
{
	alcDestroyContext(Context);
}
