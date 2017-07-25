#include "EnginePCH.hpp"

#include <al.h>

#include "SoundEmitterComponent.hpp"
#include "ResourceManager.hpp"
#include "SoundResource.hpp"

using namespace Poly;

SoundEmitterComponent::SoundEmitterComponent(const String& path, bool relative) :
	Background(relative)
{
	Resource = ResourceManager<SoundResource>::Load(path);

	alGenSources(1, &EmitterID);
	alSourcei(EmitterID, AL_BUFFER, Resource->GetBufferID());
}

SoundEmitterComponent::~SoundEmitterComponent()
{
	alDeleteSources(1, &EmitterID);

	if (Resource) 
		ResourceManager<SoundResource>::Release(Resource);
}