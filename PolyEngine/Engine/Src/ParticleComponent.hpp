#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "ParticleEmitter.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT ParticleComponent : public ComponentBase
	{
		friend void ParticleUpdateSystem::ParticleUpdatePhase(World*);
	public:
		ParticleComponent(const ParticleEmitter::Settings& emitter);
		virtual ~ParticleComponent();
		
		ParticleEmitter* GetEmitter() { return Emitter; }

		const ParticleEmitter* GetEmitter() const { return Emitter; }

	private:
		ParticleEmitter* Emitter = nullptr;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ParticleComponent)
}