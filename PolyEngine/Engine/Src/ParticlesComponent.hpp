#pragma once

#include "ComponentBase.hpp"
#include "RenderingSystem.hpp"
#include "ParticleEmitter.hpp"

namespace Poly {

	class ENGINE_DLLEXPORT ParticlesComponent : public ComponentBase
	{
	public:
		ParticlesComponent();
		virtual ~ParticlesComponent();

	private:

		ParticleEmitter* emitter;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ParticlesComponent)
}