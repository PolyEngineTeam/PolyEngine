#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "ParticleEmitter.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT ParticleComponent : public ComponentBase
	{
	public:
		ParticleComponent();
		virtual ~ParticleComponent();

		ParticleEmitter* emitter;

	private:
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ParticleComponent)
}