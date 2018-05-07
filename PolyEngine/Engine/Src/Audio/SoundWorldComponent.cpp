#include "EnginePCH.hpp"

#include "Audio/SoundWorldComponent.hpp"

#include <alc.h>

using namespace Poly;

RTTI_DEFINE_TYPE(Poly::SoundWorldComponent)

SoundWorldComponent::SoundWorldComponent(World* /*world*/)
{
	Context = alcCreateContext(gEngine->GetAudioDevice().Device, nullptr);
}

SoundWorldComponent::~SoundWorldComponent()
{
	alcDestroyContext(Context);
}