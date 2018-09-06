#pragma once

#include <Defines.hpp>
#include "ECS/ComponentBase.hpp"
#include "Audio/SoundEmitterComponent.hpp"

namespace Poly
{
	class SoundWorldComponent;

	/// Contains information about listener.
	/// There can be any number of listeners but every listener has its own copy of every emitter (except of resources)
	/// hence 10 listeners uses 10 times more CPU and memory than 1 listener.
	/// @see SoundEmitterComponent
	class ENGINE_DLLEXPORT SoundListenerComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::SoundListenerComponent) { NO_RTTI_PROPERTY(); }
		/// Sets everything to 0 and copies SourcesArray from SoundWorldComponent
		SoundListenerComponent(SoundWorldComponent* worldComponent = nullptr);
		~SoundListenerComponent();

		float GetGain() const { return Gain; }

	private:
		float Gain = 1;
		Vector Position;
		Vector Velocity;
		Dynarray<SoundEmitterComponent*> Emitters;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, SoundListenerComponent)
} // namespace Poly