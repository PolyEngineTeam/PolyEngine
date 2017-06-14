#pragma once

#include <alc.h>

#include "ComponentBase.hpp"
#include "SoundSystem.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT SoundWorldComponent : public ComponentBase
	{
	friend void SoundSystem::SetCurrentWorld(World*);
	friend void SoundSystem::CreateContext(World*);
	public:
		SoundWorldComponent();
		~SoundWorldComponent();

	private:
		ALCcontext *Context;
	};
}