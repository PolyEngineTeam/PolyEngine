#pragma once

#include <BaseObject.hpp>
#include "SoundSystem.hpp"

typedef struct ALCdevice_struct ALCdevice;

namespace Poly {

	class OpenALRenderingContext : public BaseObject<>  
	{
	friend void SoundSystem::SoundPhase(World*);
	friend void SoundSystem::CreateContext(World*);
	public:
		OpenALRenderingContext();
		~OpenALRenderingContext();

	private:
		ALCdevice* Device;
	};

} //namespace Poly