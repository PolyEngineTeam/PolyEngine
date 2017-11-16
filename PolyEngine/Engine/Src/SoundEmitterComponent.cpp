#include "EnginePCH.hpp"

using namespace Poly;

SoundEmitterComponent::SoundEmitterComponent()
{
}

SoundEmitterComponent::~SoundEmitterComponent()
{
	for (int i = 0; i < Playlist.GetSize(); i++)
		ResourceManager<SoundResource>::Release(Playlist[i]);
}
