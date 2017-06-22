#include "EnginePCH.hpp"

#include "SoundListenerComponent.hpp"

using namespace Poly;

SoundListenerComponent::SoundListenerComponent(SoundWorldComponent* worldComponent)
{
	memset(&Gain, 0, sizeof(ALfloat) * 10);
}

SoundListenerComponent::~SoundListenerComponent()
{
}
