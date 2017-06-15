#pragma once

#include "ComponentBase.hpp"
#include "SoundSystem.hpp"
#include "SoundResource.hpp"

typedef unsigned int ALuint;

namespace Poly
{
	class ENGINE_DLLEXPORT SoundEmitterComponent : public ComponentBase
	{
	friend void SoundSystem::SoundPhase(World*);
	public:
		SoundEmitterComponent(const String&);
		~SoundEmitterComponent();
	private:
		ALuint SourceID;
		SoundResource* Resource;
	};
}