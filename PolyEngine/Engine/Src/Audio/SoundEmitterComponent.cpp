#include "EnginePCH.hpp"

#include <al.h>

#include "Audio/SoundEmitterComponent.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/SoundResource.hpp"

using namespace Poly;

SoundEmitterComponent::SoundEmitterComponent(const String& path, eResourceSource source, bool relative) :
	Background(relative)
{
	Resource = ResourceManager<SoundResource>::Load(path, source);

	alGenSources(1, &EmitterID);
	alSourcei(EmitterID, AL_BUFFER, Resource->GetBufferID());
}

SoundEmitterComponent::~SoundEmitterComponent()
{
	alDeleteSources(1, &EmitterID);

	if (Resource) 
		ResourceManager<SoundResource>::Release(Resource);
}