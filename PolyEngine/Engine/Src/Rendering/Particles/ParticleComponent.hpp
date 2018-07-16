#pragma once

#include "ECS/ComponentBase.hpp"
#include "Rendering/RenderingSystem.hpp"
#include "Rendering/Particles/ParticleEmitter.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT ParticleComponent : public ComponentBase
	{
		RTTI_DECLARE_TYPE_DERIVED(ParticleComponent, ComponentBase)
		{
			NO_RTTI_PROPERTY();
		}

		friend void ParticleUpdateSystem::ParticleUpdatePhase(Scene*);
	public:
		ParticleComponent() {}
		ParticleComponent(const ParticleEmitter::Settings& emitter);
		virtual ~ParticleComponent();
		
		ParticleEmitter* GetEmitter() { return Emitter; }

		const ParticleEmitter* GetEmitter() const { return Emitter; }

	private:
		ParticleEmitter* Emitter = nullptr;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ParticleComponent)
}