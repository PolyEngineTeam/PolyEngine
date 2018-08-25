#include "EnginePCH.hpp"

#include "Audio/SoundWorldComponent.hpp"

#include <alc.h>

using namespace Poly;

RTTI_DEFINE_COMPONENT(::Poly::SoundWorldComponent)

SoundWorldComponent::SoundWorldComponent(Scene* /*world*/)
{
	Context = alcCreateContext(gEngine->GetAudioDevice().Device, nullptr);
}

SoundWorldComponent::~SoundWorldComponent()
{
	alcDestroyContext(Context);
}
