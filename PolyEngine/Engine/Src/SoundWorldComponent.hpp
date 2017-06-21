#pragma once

#include "ComponentBase.hpp"
#include "SoundSystem.hpp"

struct ALCcontext_struct;
typedef struct ALCcontext_struct ALCcontext;

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