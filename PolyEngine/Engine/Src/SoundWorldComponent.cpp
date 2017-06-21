#include "EnginePCH.hpp"

#include "SoundWorldComponent.hpp"

#include <alc.h>

using namespace Poly;

SoundWorldComponent::SoundWorldComponent()
{
}

SoundWorldComponent::~SoundWorldComponent()
{
	alcDestroyContext(Context);
}
