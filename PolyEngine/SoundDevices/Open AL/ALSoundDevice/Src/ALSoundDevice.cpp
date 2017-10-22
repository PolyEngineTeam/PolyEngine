#include "ALSoundDevice.hpp"

#include <al.h>

#include <World.hpp>
#include <TransformComponent.hpp>
#include <SoundEmitterComponent.hpp>

using namespace Poly;

void ALSoundDevice::RenderWorld(World* world)
{
	Dynarray<UniqueID> CurrentEmitterOwners;

	for (auto& emitterTuple : world->IterateComponents<SoundEmitterComponent, TransformComponent>())
	{
		SoundEmitterComponent* emitterCmp = std::get<SoundEmitterComponent*>(emitterTuple);
		TransformComponent* transCmp = std::get<TransformComponent*>(emitterTuple);
		
		UniqueID emitterOwnerID = emitterCmp->GetOwnerID();
		CurrentEmitterOwners.PushBack(emitterOwnerID);

		if (!EmitterOwners.Contains(emitterOwnerID))
		{
			EmitterOwners.PushBack(emitterOwnerID);

			unsigned int emitterID;
			alGenSources(1, &emitterID);
			OwnerIDToEmitterIDMap[emitterOwnerID] =  emitterID;
		}

		unsigned int emitterID = OwnerIDToEmitterIDMap[emitterOwnerID];

		alSourcef(emitterID, AL_PITCH, emitterCmp->Pitch);
		alSourcef(emitterID, AL_GAIN, emitterCmp->Gain);
		alSourcef(emitterID, AL_MAX_DISTANCE, emitterCmp->MaxDistance);
		alSourcef(emitterID, AL_ROLLOFF_FACTOR, emitterCmp->RolloffFactor);
		alSourcef(emitterID, AL_REFERENCE_DISTANCE, emitterCmp->RefferenceDistance);
		alSourcef(emitterID, AL_MIN_GAIN, emitterCmp->MinGain);
		alSourcef(emitterID, AL_MAX_GAIN, emitterCmp->MaxGain);
		alSourcef(emitterID, AL_CONE_OUTER_GAIN, emitterCmp->ConeOuterGain);

		ALfloat u[3] = { emitterCmp->ConeInnerAngle.X, emitterCmp->ConeInnerAngle.Y, emitterCmp->ConeInnerAngle.Z };
		alSourcefv(emitterID, AL_CONE_INNER_ANGLE, u);

		ALfloat v[3] = { emitterCmp->ConeOuterAngle.X, emitterCmp->ConeOuterAngle.Y, emitterCmp->ConeOuterAngle.Z };
		alSourcefv(emitterID, AL_CONE_OUTER_ANGLE, v);

		ALfloat w[3] = { emitterCmp->Direction.X, emitterCmp->Direction.Y, emitterCmp->Direction.Z };
		alSourcefv(emitterID, AL_CONE_OUTER_GAIN, w);

		Vector pos = transCmp->GetGlobalTranslation();
		alSource3f(emitterID, AL_POSITION, pos.X, pos.Y, pos.Z);

		if (emitterCmp->Looping)
			alSourcei(emitterID, AL_LOOPING, AL_TRUE);
		else
			alSourcei(emitterID, AL_LOOPING, AL_FALSE);

		if (emitterCmp->Paused)
			alSourcei(emitterID, AL_SOURCE_STATE, AL_PAUSED);
		else
			alSourcei(emitterID, AL_SOURCE_STATE, AL_PLAYING);

		// set velocity
	}

	for (int i = 0; i < EmitterOwners.GetSize(); i++)
		if (!CurrentEmitterOwners.Contains(EmitterOwners[i]))
		{
			// delete emitter from openal and from emitterOwners list
		}
}
