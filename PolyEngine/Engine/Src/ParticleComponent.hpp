#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "ParticleEmitter.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT ParticleComponent : public ComponentBase
	{
	public:
		ParticleComponent(ParticleEmitter::Settings emitter);
		virtual ~ParticleComponent();

		ParticleEmitter* Emitter;

	private:
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ParticleComponent)
}