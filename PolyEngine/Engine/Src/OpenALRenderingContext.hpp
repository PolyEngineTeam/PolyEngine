#pragma once

//struct ALCdevice;
typedef struct ALCdevice_struct ALCdevice;

#include <BaseObject.hpp>
#include "SoundSystem.hpp"

namespace Poly {

	class OpenALRenderingContext : public BaseObject<>  
	{
	friend void SoundSystem::SoundPhase(World*);
	public:
		OpenALRenderingContext();
		~OpenALRenderingContext();

		void Init();
		void EndFrame();

		ALCdevice* GetDevice() { return Device; };

	private:
		ALCdevice* Device;
	};

} //namespace Poly