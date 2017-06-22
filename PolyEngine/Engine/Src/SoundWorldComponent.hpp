#pragma once

#include "ComponentBase.hpp"
#include "SoundSystem.hpp"

typedef struct ALCcontext_struct ALCcontext;

namespace Poly
{
	class World;

	/// Component containing OpenAL context used in sound rendering.
	/// Only one context can be active in time.
	/// There can be only one context per world.
	/// @see SoundSystem
	class ENGINE_DLLEXPORT SoundWorldComponent : public ComponentBase
	{
	friend void SoundSystem::SetCurrentWorld(World* world);
	public:
		/// Creates context from current device (stored in engine).
		/// @param world - world which will  be associated with just created context.
		/// @see OpenALRenderingDevice
		SoundWorldComponent(World* world);

		/// Releases stored context.
		~SoundWorldComponent();

		const ALCcontext* GetContext() const { return Context; }

	private:
		ALCcontext* Context;
	};

} // namespace Poly