#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Rendering/RenderingSystem.hpp>
#include <Rendering/Particles/ParticleEmitter.hpp>

namespace Poly {

	class ENGINE_DLLEXPORT ParticleComponent : public ComponentBase
	{
		friend void ParticleUpdateSystem::ParticleUpdatePhase(Scene*);
	public:
		RTTI_DECLARE_COMPONENT(::Poly::ParticleComponent) { NO_RTTI_PROPERTY(); }

		ParticleComponent(const ParticleEmitter::Settings& emitter);
		virtual ~ParticleComponent();
		
		ParticleEmitter* GetEmitter() { return Emitter; }

		const ParticleEmitter* GetEmitter() const { return Emitter; }

	private:
		ParticleEmitter* Emitter = nullptr;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, ParticleComponent)
}