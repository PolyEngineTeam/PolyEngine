#pragma once

typedef unsigned int ALuint;

#include "ComponentBase.hpp"
#include "SoundSystem.hpp"
#include "SoundResource.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT SoundEmitterComponent : public ComponentBase
	{
	friend void SoundSystem::SoundPhase(World*);
	public:
		SoundEmitterComponent(const String&);
		~SoundEmitterComponent();

		ALuint GetSourceID() const { return SourceID; }
		SoundResource* GetResource() const { return Resource; }
	private:
		ALuint SourceID;
		SoundResource* Resource;
	};
}