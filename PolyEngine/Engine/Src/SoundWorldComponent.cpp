#include "EnginePCH.hpp"

#include "SoundWorldComponent.hpp"

using namespace Poly;

SoundWorldComponent::SoundWorldComponent()
{
}

SoundWorldComponent::~SoundWorldComponent()
{
	alcDestroyContext(Context);
}
