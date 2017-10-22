#include "EnginePCH.hpp"

#include "SoundWorldComponent.hpp"

#include <alc.h>

using namespace Poly;

SoundWorldComponent::SoundWorldComponent(World* /*world*/)
{
	Context = alcCreateContext(gEngine->GetSoundDevice().Device, nullptr);
}

SoundWorldComponent::~SoundWorldComponent()
{
	alcDestroyContext(Context);
}
