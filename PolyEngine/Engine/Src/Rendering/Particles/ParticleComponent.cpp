#include "EnginePCH.hpp"

#include "ParticleComponent.hpp"
#include "CameraComponent.hpp"

using namespace Poly;

ParticleComponent::ParticleComponent(const ParticleEmitter::Settings& emitter)
{
	Emitter = new ParticleEmitter(emitter);
}

Poly::ParticleComponent::~ParticleComponent()
{
	delete Emitter;
}