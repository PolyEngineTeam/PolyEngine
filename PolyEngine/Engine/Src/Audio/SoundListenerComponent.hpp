#pragma once

#include "ECS/ComponentBase.hpp"
#include <Math/Vector.hpp>
#include <Collections/Dynarray.hpp>
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
		RTTI_DECLARE_TYPE_DERIVED(AssetsPathConfig, ConfigBase)
		{
			RTTI_PROPERTY(Gain, "Gain", RTTI::ePropertyFlag::NONE);
			//RTTI_PROPERTY(Position, "Position", RTTI::ePropertyFlag::NONE);
			//RTTI_PROPERTY(Velocity, "Velocity", RTTI::ePropertyFlag::NONE);
			//RTTI_PROPERTY(Emitters, "Emitters", RTTI::ePropertyFlag::NONE);
		}

	public:
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

} // namespace Poly