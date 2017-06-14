#include "EnginePCH.hpp"

#include <al.h>

#include "SoundEmitterComponent.hpp"
#include "ResourceManager.hpp"

using namespace Poly;

SoundEmitterComponent::SoundEmitterComponent(const String& path)
{
	Resource = ResourceManager<SoundResource>::Load(path);

	alGenSources(1, &SourceID);
	alSourcei(SourceID, AL_BUFFER, Resource->GetBufferID());
}

SoundEmitterComponent::~SoundEmitterComponent()
{
	if (Resource) 
		ResourceManager<SoundResource>::Release(Resource);
}
